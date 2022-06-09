# Scalable Rendering Project
---
## Build

- Visual Studio 2022 (should work with the previous ones)
- Eigen 3.3.3 (using NuGet Package Manager)
- ImGUI (using GLUT+OpenGL2 stack)

---

## Tasks

With respect to the list of tasks to be delivered, the status of these are the following:

- Basic tasks:
  - Compute Framerate & Display: **DONE** (using IMGUI!)
  - Basic Museum: **DONE**
  - Compute simplified versions of the loaded model: **DONE**
    - Representative vertex per cluster: Mean
    - 4 LODs (chosen sizes: 14, 11, 8, 5)
  - Compute vertex representative using QEM: **In Progress**
    - Sum Quadrics of each vertex: **DONE**
    - For each cluster, sum the Quadrics: **DONE**
    - For the sum of Quadrics, calculate the position with less error: **Fails**. The project is using the JacobiSVD from the Eigen library, but the results is almost always near zero for each position.
    - Time-Critical Rendering: **In Progress**
      - Fix Values for MaxCost in ImGUI: **TO DO**
      - Set structure for performing Benefit Cost Knapsack Problem: **DONE**
      - Solve the problem using heuristics: **In Progress**. Using the Greedy approach.
    - Precompute cell-to-cell visibility: **TO DO**
  

- Advanced tasks:
  - Complex floorplan: **DONE**
  - Use an Octree to generate all simplified models: **DONE**
  - Hysteresis transition: **In progress**
  - Preprocess but can be optimized via Octree ray traversal: **TO DO**
