# CPPathfinder

*Pathfinding* is a method to **find the shortest and most efficient route between two points on a map**. It has numerous applications such as powering Google Maps, video games NPCs' AI, etc.

<br>
This program aims to help understanding how *Pathfinding* works by demonstrating four algorithms: DFS, BFS, Dijkstra's and A*. It shows their execution, all the explored routes, the final chosen path and even a step by step visualizer to show how these algos explore the map.

#### Maps

The program takes 2D ASCII maps as input. Sample maps are provided in [./maps/](./maps/). They must be rectangular. Symbols are:
`#`: walls
`S`: Starting point
`E`: Ending point
` `: Normal walkable route
`:`: Medium walkable route (more costly)
`;`: Hard walkable route (even more costly)

The output is rendered into a window with tiles to represent the maps more nicely. Grey tiles are walls, Green/Red is Start/End and Blue show the final path. Yellow tiles represent the routes the algorithm has explored while it was searching for the path.


#### Algos

- Depth-First Search blindly explores as far as possible in a single direction, only backtracking to the last intersection when it hits a wall or dead end.
- Breadth-First Search explores in concentric waves, visiting every neighbor route at the current distance before moving one step further out. That ensures the shortest past in an unweighted map.
- Dijkstra's systematically expands towards the cheapest neighbor routes first, constantly updating the cumulative "cost" to reach each point. That ensures the most efficient route in a weighted map.
- A* combines Dijkstra's cost calculation with a Heuristic (an educated guess) of the distance to the Ending Point. It will lose less time exploring the map thus finding the most efficient path faster.


#### Installation

Download the compressed folder from the [Releases](/releases) tab. You need all the included files in order to be able to run the program.

Alternatively, you can also download the source code and compile it yourself using
`g++ src/main.cpp -I./include -lsfml-graphics -lsfml-window -lsfml-system -o ./build/cppathfinder`
You will need to have SFML installed.


#### Usage

You need to provide the map to the program when running it using the `-m` option:
`./cppathfinder -m path/to/your/map.txt`

Three modes are available:
- Default (no extra flag)
- Visited Mode (`--show-visited`)
- Visualize Mode (`--visualize INT`)

##### Visited Mode

`./cppathfinder -m path/to/your/map.txt --show-visited`
This will display all the routes that the algorithm has explored while it was searching for the path.

##### Visualize Mode

`./cppathfinder -m path/to/your/map.txt --visualize 50`
This will run the algorithm step by step, showing all the routes being explored, to demonstrate how the algo is behaving. You can set the step delay (in ms) by passing the argument (default is 50).

<br>

⚠️ **Warning** ⚠️
You always need to include the `arial.ttf` file alongside the executable or the program will not be able to run. This is because the graphics library used relies on external fonts that cannot be embedded into the program.