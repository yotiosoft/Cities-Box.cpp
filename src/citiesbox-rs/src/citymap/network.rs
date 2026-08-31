//! Value-based connectable network analysis.
//!
//! The current save format does not persist connection edges, so this module
//! accepts a snapshot reconstructed by C++ and never becomes a second source
//! of truth. It deliberately contains no OpenSiv3D types or C++ pointers.

use std::collections::{BTreeMap, BTreeSet};

use super::ffi::{ConnectableNetworkAnalysis, ConnectableNetworkEdge, ConnectableNetworkNode};

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
}
