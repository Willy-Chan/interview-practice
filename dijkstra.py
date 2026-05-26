import heapq

# INTUITION:
#   Look at your neighbors
#   Pick the one with the shortest path
#   Now look at that neighbor's neighbors. Those "extend" to form new paths.
#   Pick the smallest out of [neighbor's neighbors] + [other stage 1 neighbors you didn't consider]


# IMPLEMENTATION:
# initialize dict: distance from [start_node] to everyone else is infinity:   {dist_to_node_1 : inf, dist_to_node_2 : inf, ...}
# maintain a heap of (distance_to_node, node)
# while heap:
#       pop the smallest known distance + node
#       for all of that node's neighbors:
#           calculate newest distance
#           if distance smaller for that neighbor: assign it to dict, and push to top of heap

# gives you SMALLEST DISTANCES TO ALL NODES!!!

def dijkstra(graph, start):
    # Step 1: Initialize all distances to infinity, and the start node to 0
    distances = {node: float('inf') for node in graph}
    distances[start] = 0
    
    # The priority queue will hold tuples of: (distance_to_node, node)
    # It automatically sorts by the first element (distance)
    priority_queue = [(0, start)]
    
    while priority_queue:
        # Step 2: Pop the node with the smallest known distance
        current_distance, current_node = heapq.heappop(priority_queue)
        
        # Guard clause: If we found a shorter path to this node already while 
        # it was waiting in the queue, skip processing its neighbors again.
        if current_distance > distances[current_node]:
            continue
            
        # Step 3 & 4: Explore neighbors
        for neighbor, weight in graph[current_node].items():
            distance = current_distance + weight
            
            # If this new path to the neighbor is shorter, adopt it!
            if distance < distances[neighbor]:
                distances[neighbor] = distance
                # Push the updated distance to the queue
                heapq.heappush(priority_queue, (distance, neighbor))
                
    return distances

# --- Example Usage ---

# Representing a graph using an adjacency list (dictionary of dictionaries)
# Node: {Neighbor: Weight/Distance}
city_map = {
    'Home': {'A': 4, 'B': 2},
    'A': {'B': 1, 'C': 5},
    'B': {'A': 1, 'C': 8, 'D': 10},
    'C': {'D': 2},
    'D': {}
}

shortest_paths = dijkstra(city_map, 'Home')
print(shortest_paths)
# Output: {'Home': 0, 'A': 3, 'B': 2, 'C': 8, 'D': 10}