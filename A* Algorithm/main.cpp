// A*Algorithm
#include <iostream>
#include <queue>
#include <cmath>

using namespace std;

typedef struct CoordinateStruct {
    int x;
    int y;
} CoordinateStruct;

typedef struct Node {
    int node_id;
    CoordinateStruct coordinate;

    int f = 0;

    vector<Node*> connection;

    bool operator<(const Node &b) const {
        return (f<b.f);
    }
    bool operator>(const Node &b) const {
        return (f>b.f);
    }
} Node;

class AStar {
public:
    AStar(vector<Node> &new_graph, int start_id) {
        network = new_graph;

        // スタートノードをopenに追加する
        for (int i=0; i<network.size(); i++) {
            if (network[i].node_id == start_id) {
                open.push(&network[i]);
                start_node = &network[i];
                break;
            }
            close.push(&network[i]);
        }

        for (int i=0; i<network.size(); i++) {
            cout << close.top()->f << endl;
        }

        if (open.size() == 0) {
            cerr << "AStar: Queue Set Failed" << endl;
            return;
        }
    }

    double h(int id1, int id2) {
        int num1 = -1, num2 = -2;
        for (int i=0; i<network.size(); i++) {
            if (network[i].node_id == id1) {
                num1 = i;
            }
            if (network[i].node_id == id2) {
                num2 = i;
            }
            if (num1 >= 0 && num2 >= 0) {
                break;
            }
        }

        if (num1 == -1 || num2 == -2) {
            return INFINITY;
        }

        double x_distance = network[num1].coordinate.x-network[num2].coordinate.x;
        double y_distance = network[num1].coordinate.y-network[num2].coordinate.y;
        return sqrt(pow(x_distance, 2)+pow(y_distance, 2));
    }

private:
    vector<Node> network;
    priority_queue<Node*> open;
    priority_queue<Node*> close;
    Node* start_node;
};

int main() {
    vector<Node> network;

    network.push_back(Node{1, 0, 0, 3});
    network.push_back(Node{2, 1, 0, 2});
    network.push_back(Node{3, 0, 1, 4});
    network.push_back(Node{4, 1, 1, 1});

    network[0].connection.push_back(&network[1]);
    network[0].connection.push_back(&network[2]);

    network[1].connection.push_back(&network[0]);

    network[2].connection.push_back(&network[0]);
    network[2].connection.push_back(&network[3]);

    network[3].connection.push_back(&network[2]);

    AStar a_star(network, 1);

    return 0;
}
