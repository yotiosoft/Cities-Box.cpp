//! Value-based connectable network analysis.
//!
//! The current save format does not persist connection edges, so this module
//! accepts value snapshots and incremental mutations. It deliberately contains
//! no OpenSiv3D types or C++ pointers.

use std::collections::{BTreeMap, BTreeSet};

use super::ffi::{ConnectableNetworkAnalysis, ConnectableNetworkEdge, ConnectableNetworkNode};

#[derive(Clone, Debug)]
struct NetworkNode {
    x: i32,
    y: i32,
    connectable_kind: i32,
    direction_id: i32,
    category_ids: Vec<i32>,
    under_construction: bool,
}

#[derive(Clone, Copy, Debug)]
struct NetworkEdge {
    from_direction: i32,
    to_direction: i32,
}

#[derive(Debug, Default)]
pub(crate) struct ConnectableNetwork {
    nodes: BTreeMap<i32, NetworkNode>,
    edges: BTreeMap<(i32, i32), NetworkEdge>,
}

impl ConnectableNetwork {
    pub(crate) fn upsert_node(&mut self, node: ConnectableNetworkNode) {
        let under_construction = self
            .nodes
            .get(&node.object_id)
            .is_some_and(|current| current.under_construction)
            || node.under_construction;
        self.nodes.insert(
            node.object_id,
            NetworkNode {
                x: node.x,
                y: node.y,
                connectable_kind: node.connectable_kind,
                direction_id: node.direction_id,
                category_ids: node.category_ids,
                under_construction,
            },
        );
    }

    pub(crate) fn connect(&mut self, edge: ConnectableNetworkEdge) -> bool {
        if edge.from_object_id == edge.to_object_id
            || !self.nodes.contains_key(&edge.from_object_id)
            || !self.nodes.contains_key(&edge.to_object_id)
        {
            return false;
        }

        let (key, value) = if edge.from_object_id < edge.to_object_id {
            (
                (edge.from_object_id, edge.to_object_id),
                NetworkEdge {
                    from_direction: edge.from_direction,
                    to_direction: edge.to_direction,
                },
            )
        } else {
            (
                (edge.to_object_id, edge.from_object_id),
                NetworkEdge {
                    from_direction: edge.to_direction,
                    to_direction: edge.from_direction,
                },
            )
        };
        self.edges.insert(key, value);
        self.nodes
            .get_mut(&edge.from_object_id)
            .expect("validated node")
            .under_construction = false;
        self.nodes
            .get_mut(&edge.to_object_id)
            .expect("validated node")
            .under_construction = false;
        true
    }

    pub(crate) fn remove_node(&mut self, object_id: i32) -> bool {
        let removed = self.nodes.remove(&object_id).is_some();
        self.edges
            .retain(|(from, to), _| *from != object_id && *to != object_id);
        removed
    }

    pub(crate) fn take_unfinished_isolated_object_ids(&mut self) -> Vec<i32> {
        let nodes = self
            .nodes
            .iter()
            .map(|(object_id, node)| ConnectableNetworkNode {
                object_id: *object_id,
                x: node.x,
                y: node.y,
                connectable_kind: node.connectable_kind,
                direction_id: node.direction_id,
                category_ids: node.category_ids.clone(),
                under_construction: node.under_construction,
            })
            .collect();
        let edges = self
            .edges
            .iter()
            .map(|((from, to), edge)| ConnectableNetworkEdge {
                from_object_id: *from,
                to_object_id: *to,
                from_direction: edge.from_direction,
                to_direction: edge.to_direction,
            })
            .collect();
        let unfinished = analyze(nodes, edges).unfinished_isolated_object_ids;
        for node in self.nodes.values_mut() {
            node.under_construction = false;
        }
        unfinished
    }

    pub(crate) fn rebuild(
        &mut self,
        nodes: Vec<ConnectableNetworkNode>,
    ) -> ConnectableNetworkAnalysis {
        self.nodes.clear();
        self.edges.clear();
        for node in nodes {
            self.upsert_node(node);
        }

        let mut positions = BTreeMap::<(i32, i32), Vec<i32>>::new();
        for (id, node) in &self.nodes {
            positions.entry((node.x, node.y)).or_default().push(*id);
        }
        let object_ids = self.nodes.keys().copied().collect::<Vec<_>>();
        for from_id in object_ids {
            let Some(from) = self.nodes.get(&from_id).cloned() else {
                continue;
            };
            for from_direction in atomic_directions(from.direction_id) {
                let (dx, dy, to_direction) = direction_step(from_direction);
                let Some(to_ids) = positions.get(&(from.x + dx, from.y + dy)) else {
                    continue;
                };
                for &to_id in to_ids {
                    let Some(to) = self.nodes.get(&to_id) else {
                        continue;
                    };
                    if atomic_directions(to.direction_id).contains(&to_direction)
                        && super::connectable::categories_can_connect(
                            &from.category_ids,
                            &to.category_ids,
                        )
                    {
                        self.connect(ConnectableNetworkEdge {
                            from_object_id: from_id,
                            to_object_id: to_id,
                            from_direction,
                            to_direction,
                        });
                    }
                }
            }
        }

        self.analysis()
    }

    fn analysis(&self) -> ConnectableNetworkAnalysis {
        let nodes = self
            .nodes
            .iter()
            .map(|(object_id, node)| ConnectableNetworkNode {
                object_id: *object_id,
                x: node.x,
                y: node.y,
                connectable_kind: node.connectable_kind,
                direction_id: node.direction_id,
                category_ids: node.category_ids.clone(),
                under_construction: node.under_construction,
            })
            .collect();
        let edges = self
            .edges
            .iter()
            .map(|((from, to), edge)| ConnectableNetworkEdge {
                from_object_id: *from,
                to_object_id: *to,
                from_direction: edge.from_direction,
                to_direction: edge.to_direction,
            })
            .collect();
        analyze(nodes, edges)
    }

    #[cfg(test)]
    fn edge(&self, from: i32, to: i32) -> Option<(i32, i32)> {
        let edge = self.edges.get(&(from.min(to), from.max(to)))?;
        Some(if from < to {
            (edge.from_direction, edge.to_direction)
        } else {
            (edge.to_direction, edge.from_direction)
        })
    }

    #[cfg(test)]
    fn node_values(&self, object_id: i32) -> Option<(i32, i32, i32)> {
        let node = self.nodes.get(&object_id)?;
        Some((node.x, node.y, node.connectable_kind))
    }
}

fn atomic_directions(direction: i32) -> Vec<i32> {
    use super::connectable::direction_id as d;

    let all = [
        d::NORTH,
        d::NORTHEAST,
        d::EAST,
        d::SOUTHEAST,
        d::SOUTH,
        d::SOUTHWEST,
        d::WEST,
        d::NORTHWEST,
    ];
    match direction {
        d::NONE | d::DISABLED => Vec::new(),
        d::NORTH => vec![d::NORTH],
        d::SOUTH => vec![d::SOUTH],
        d::EAST => vec![d::EAST],
        d::WEST => vec![d::WEST],
        d::EAST_WEST => vec![d::EAST, d::WEST],
        d::NORTH_SOUTH => vec![d::NORTH, d::SOUTH],
        d::SOUTH_WEST => vec![d::SOUTH, d::WEST],
        d::NORTH_WEST => vec![d::NORTH, d::WEST],
        d::SOUTH_EAST => vec![d::SOUTH, d::EAST],
        d::NORTH_EAST => vec![d::NORTH, d::EAST],
        d::SOUTH_EAST_WEST => vec![d::SOUTH, d::EAST, d::WEST],
        d::NORTH_EAST_WEST => vec![d::NORTH, d::EAST, d::WEST],
        d::NORTH_SOUTH_WEST => vec![d::NORTH, d::SOUTH, d::WEST],
        d::NORTH_SOUTH_EAST => vec![d::NORTH, d::SOUTH, d::EAST],
        d::ALL => vec![d::NORTH, d::EAST, d::SOUTH, d::WEST],
        d::NORTHEAST | d::NORTHWEST | d::SOUTHEAST | d::SOUTHWEST => vec![direction],
        d::OFFSHORE => all.to_vec(),
        d::WITHOUT_SOUTHWEST_NORTHWEST => all
            .into_iter()
            .filter(|value| ![d::SOUTHWEST, d::NORTHWEST].contains(value))
            .collect(),
        d::WITHOUT_NORTHEAST_NORTHWEST => all
            .into_iter()
            .filter(|value| ![d::NORTHEAST, d::NORTHWEST].contains(value))
            .collect(),
        d::WITHOUT_SOUTHEAST_SOUTHWEST => all
            .into_iter()
            .filter(|value| ![d::SOUTHEAST, d::SOUTHWEST].contains(value))
            .collect(),
        d::WITHOUT_NORTHEAST_SOUTHEAST => all
            .into_iter()
            .filter(|value| ![d::NORTHEAST, d::SOUTHEAST].contains(value))
            .collect(),
        d::WITHOUT_EAST => all
            .into_iter()
            .filter(|value| ![d::NORTHEAST, d::EAST, d::SOUTHEAST].contains(value))
            .collect(),
        d::WITHOUT_SOUTH => all
            .into_iter()
            .filter(|value| ![d::SOUTHEAST, d::SOUTH, d::SOUTHWEST].contains(value))
            .collect(),
        d::WITHOUT_NORTH => all
            .into_iter()
            .filter(|value| ![d::NORTH, d::NORTHEAST, d::NORTHWEST].contains(value))
            .collect(),
        d::WITHOUT_WEST => all
            .into_iter()
            .filter(|value| ![d::SOUTHWEST, d::WEST, d::NORTHWEST].contains(value))
            .collect(),
        d::WITHOUT_NORTH_WEST_NORTHWEST => all
            .into_iter()
            .filter(|value| ![d::NORTH, d::WEST, d::NORTHWEST].contains(value))
            .collect(),
        d::WITHOUT_NORTH_NORTHEAST_EAST => all
            .into_iter()
            .filter(|value| ![d::NORTH, d::NORTHEAST, d::EAST].contains(value))
            .collect(),
        d::WITHOUT_SOUTH_SOUTHWEST_WEST => all
            .into_iter()
            .filter(|value| ![d::SOUTH, d::SOUTHWEST, d::WEST].contains(value))
            .collect(),
        d::WITHOUT_EAST_SOUTHEAST_SOUTH => all
            .into_iter()
            .filter(|value| ![d::EAST, d::SOUTHEAST, d::SOUTH].contains(value))
            .collect(),
        d::WITHOUT_NORTHWEST => all
            .into_iter()
            .filter(|value| *value != d::NORTHWEST)
            .collect(),
        d::WITHOUT_NORTHEAST => all
            .into_iter()
            .filter(|value| *value != d::NORTHEAST)
            .collect(),
        d::WITHOUT_SOUTHWEST => all
            .into_iter()
            .filter(|value| *value != d::SOUTHWEST)
            .collect(),
        d::WITHOUT_SOUTHEAST => all
            .into_iter()
            .filter(|value| *value != d::SOUTHEAST)
            .collect(),
        _ => Vec::new(),
    }
}

fn direction_step(direction: i32) -> (i32, i32, i32) {
    use super::connectable::direction_id as d;
    match direction {
        d::NORTH => (0, -1, d::SOUTH),
        d::SOUTH => (0, 1, d::NORTH),
        d::EAST => (1, 0, d::WEST),
        d::WEST => (-1, 0, d::EAST),
        d::NORTHEAST => (1, -1, d::SOUTHWEST),
        d::NORTHWEST => (-1, -1, d::SOUTHEAST),
        d::SOUTHEAST => (1, 1, d::NORTHWEST),
        d::SOUTHWEST => (-1, 1, d::NORTHEAST),
        _ => (0, 0, d::DISABLED),
    }
}

pub(crate) fn analyze(
    nodes: Vec<ConnectableNetworkNode>,
    edges: Vec<ConnectableNetworkEdge>,
) -> ConnectableNetworkAnalysis {
    let mut node_indices = BTreeMap::new();
    let mut duplicate_object_ids = BTreeSet::new();

    for (index, node) in nodes.iter().enumerate() {
        if node_indices.insert(node.object_id, index).is_some() {
            duplicate_object_ids.insert(node.object_id);
        }
    }

    let mut adjacency = vec![BTreeSet::new(); nodes.len()];
    let mut invalid_edge_indices = Vec::new();
    for (edge_index, edge) in edges.iter().enumerate() {
        let Some(&from_index) = node_indices.get(&edge.from_object_id) else {
            invalid_edge_indices.push(edge_index as i32);
            continue;
        };
        let Some(&to_index) = node_indices.get(&edge.to_object_id) else {
            invalid_edge_indices.push(edge_index as i32);
            continue;
        };
        if from_index == to_index
            || duplicate_object_ids.contains(&edge.from_object_id)
            || duplicate_object_ids.contains(&edge.to_object_id)
        {
            invalid_edge_indices.push(edge_index as i32);
            continue;
        }
        adjacency[from_index].insert(to_index);
        adjacency[to_index].insert(from_index);
    }

    let mut component_ids = vec![-1; nodes.len()];
    let mut component_count = 0;
    for start in 0..nodes.len() {
        if component_ids[start] >= 0 || duplicate_object_ids.contains(&nodes[start].object_id) {
            continue;
        }
        let mut stack = vec![start];
        component_ids[start] = component_count;
        while let Some(current) = stack.pop() {
            for &next in &adjacency[current] {
                if component_ids[next] < 0 {
                    component_ids[next] = component_count;
                    stack.push(next);
                }
            }
        }
        component_count += 1;
    }

    let isolated_object_ids = nodes
        .iter()
        .enumerate()
        .filter(|(index, node)| {
            adjacency[*index].is_empty() && !duplicate_object_ids.contains(&node.object_id)
        })
        .map(|(_, node)| node.object_id)
        .collect::<Vec<_>>();
    let unfinished_isolated_object_ids = nodes
        .iter()
        .filter(|node| node.under_construction && isolated_object_ids.contains(&node.object_id))
        .map(|node| node.object_id)
        .collect();

    ConnectableNetworkAnalysis {
        component_count,
        component_ids,
        isolated_object_ids,
        unfinished_isolated_object_ids,
        duplicate_object_ids: duplicate_object_ids.into_iter().collect(),
        invalid_edge_indices,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn node(object_id: i32, under_construction: bool) -> ConnectableNetworkNode {
        ConnectableNetworkNode {
            object_id,
            x: object_id,
            y: 0,
            connectable_kind: 2,
            direction_id: super::super::connectable::direction_id::NONE,
            category_ids: vec![1, 2],
            under_construction,
        }
    }

    fn edge(from: i32, to: i32) -> ConnectableNetworkEdge {
        ConnectableNetworkEdge {
            from_object_id: from,
            to_object_id: to,
            from_direction: 5,
            to_direction: 9,
        }
    }

    #[test]
    fn finds_components_and_isolated_construction_nodes() {
        let result = analyze(
            vec![
                node(1, false),
                node(2, false),
                node(3, true),
                node(4, false),
            ],
            vec![edge(1, 2)],
        );

        assert_eq!(result.component_count, 3);
        assert_eq!(result.component_ids, vec![0, 0, 1, 2]);
        assert_eq!(result.isolated_object_ids, vec![3, 4]);
        assert_eq!(result.unfinished_isolated_object_ids, vec![3]);
        assert!(result.invalid_edge_indices.is_empty());
    }

    #[test]
    fn treats_duplicate_edges_as_one_undirected_connection() {
        let result = analyze(
            vec![node(10, false), node(20, false)],
            vec![edge(10, 20), edge(20, 10), edge(10, 20)],
        );

        assert_eq!(result.component_count, 1);
        assert!(result.isolated_object_ids.is_empty());
        assert!(result.invalid_edge_indices.is_empty());
    }

    #[test]
    fn reports_unknown_self_and_ambiguous_edges() {
        let result = analyze(
            vec![node(1, false), node(1, false), node(2, false)],
            vec![edge(1, 2), edge(2, 2), edge(2, 99)],
        );

        assert_eq!(result.duplicate_object_ids, vec![1]);
        assert_eq!(result.invalid_edge_indices, vec![0, 1, 2]);
        assert_eq!(result.component_count, 1);
        assert_eq!(result.component_ids, vec![-1, -1, 0]);
        assert_eq!(result.isolated_object_ids, vec![2]);
    }

    #[test]
    fn empty_snapshot_has_no_components() {
        let result = analyze(Vec::new(), Vec::new());
        assert_eq!(result.component_count, 0);
        assert!(result.component_ids.is_empty());
    }

    #[test]
    fn incremental_network_owns_construction_lifecycle() {
        let mut network = ConnectableNetwork::default();
        network.upsert_node(node(10, true));
        network.upsert_node(node(20, true));
        network.upsert_node(node(30, true));

        assert!(network.connect(edge(10, 20)));
        assert_eq!(network.edge(10, 20), Some((5, 9)));
        assert_eq!(network.edge(20, 10), Some((9, 5)));
        assert_eq!(network.take_unfinished_isolated_object_ids(), vec![30]);
        assert!(network.take_unfinished_isolated_object_ids().is_empty());
    }

    #[test]
    fn upsert_preserves_construction_until_a_connection_is_added() {
        let mut network = ConnectableNetwork::default();
        network.upsert_node(node(7, true));
        let mut refreshed = node(7, false);
        refreshed.x = 40;
        refreshed.y = 50;
        refreshed.connectable_kind = 9;
        network.upsert_node(refreshed);

        assert_eq!(network.node_values(7), Some((40, 50, 9)));
        assert_eq!(network.take_unfinished_isolated_object_ids(), vec![7]);
    }

    #[test]
    fn removing_a_node_removes_its_edges() {
        let mut network = ConnectableNetwork::default();
        network.upsert_node(node(1, false));
        network.upsert_node(node(2, false));
        assert!(network.connect(edge(1, 2)));

        assert!(network.remove_node(1));
        assert!(network.edge(1, 2).is_none());
        assert!(!network.remove_node(1));
    }

    #[test]
    fn rebuilds_reciprocal_cardinal_connections() {
        use super::super::connectable::direction_id as d;
        let mut network = ConnectableNetwork::default();
        let mut west = node(1, false);
        west.x = 4;
        west.direction_id = d::EAST;
        let mut east = node(2, false);
        east.x = 5;
        east.direction_id = d::WEST;

        let analysis = network.rebuild(vec![west, east]);

        assert_eq!(analysis.component_count, 1);
        assert_eq!(network.edge(1, 2), Some((d::EAST, d::WEST)));
    }

    #[test]
    fn rebuild_requires_reciprocal_direction_and_compatible_categories() {
        use super::super::connectable::{category_id as c, direction_id as d};
        let mut network = ConnectableNetwork::default();
        let mut road = node(1, false);
        road.x = 4;
        road.direction_id = d::EAST;
        let mut one_sided = node(2, false);
        one_sided.x = 5;
        one_sided.direction_id = d::EAST;
        let mut water = node(3, false);
        water.x = 5;
        water.direction_id = d::WEST;
        water.category_ids = vec![c::CONNECTABLE, c::WATERWAY];

        let analysis = network.rebuild(vec![road, one_sided, water]);

        assert_eq!(analysis.component_count, 3);
        assert!(network.edge(1, 2).is_none());
        assert!(network.edge(1, 3).is_none());
    }

    #[test]
    fn rebuilds_diagonal_waterway_connections() {
        use super::super::connectable::{category_id as c, direction_id as d};
        let mut network = ConnectableNetwork::default();
        let mut northwest = node(1, false);
        northwest.x = 4;
        northwest.y = 4;
        northwest.direction_id = d::SOUTHEAST;
        northwest.category_ids = vec![c::CONNECTABLE, c::WATERWAY];
        let mut southeast = node(2, false);
        southeast.x = 5;
        southeast.y = 5;
        southeast.direction_id = d::NORTHWEST;
        southeast.category_ids = vec![c::CONNECTABLE, c::WATERWAY];

        network.rebuild(vec![northwest, southeast]);

        assert_eq!(network.edge(1, 2), Some((d::SOUTHEAST, d::NORTHWEST)));
    }
}
