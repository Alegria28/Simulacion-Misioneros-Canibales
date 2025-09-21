# 🚢 Missionaries and Cannibals Problem Simulation 🌊

## 📸 Simulation Images

Here are some representative screenshots of the simulation:

### Main Menu 🏠
![Main Menu](imagenes/menu.png)

### Problem Resolution 🧠
![Problem Resolution](imagenes/resolucion.png)

### Final Screen 🎉
![Final Screen](imagenes/final.png)

This project is a graphical simulation of the classic Missionaries and Cannibals logic problem, implemented in C++ using the Allegro 5 library. The program allows the user to define the initial number of missionaries and cannibals, and then visualizes the step-by-step solution, if one exists.

## ✨ Features

*   **Problem Solving:** Implements a search algorithm (DFS) to find a solution to the Missionaries and Cannibals problem.
*   **Graphical Visualization:** Uses Allegro 5 to draw the state of the problem, including the riverbanks, the boat, and the figures of the missionaries and cannibals.
*   **Solution Animation:** Displays the sequence of movements of the found solution in an animated way.
*   **User Input:** Allows the user to specify the initial number of missionaries and cannibals.
*   **Speed Control:** The user can adjust the speed of the solution animation.
*   **Invalid State Detection:** Verifies the validity of the states to ensure that missionaries are not outnumbered by cannibals on either bank.
*   **Results File Generation:** When a solution is found, a `results.txt` file is automatically generated with the complete sequence of steps.
*   **Navigation Controls:** Includes options to return to the main menu and restart the simulation without needing to load files.
*   **Interactive Interface:** Guides the user through different phases: data entry, resolution/animation, and final state (solution found or no solution).

## ⚙️ Requirements

*   A C++ compiler (like g++)
*   Allegro 5

Make sure you have Allegro 5 installed on your system.

**For Ubuntu 18.04+ (or derivatives):**

First, add the Allegro PPA. This will give you updated versions of Allegro:
```bash
sudo add-apt-repository ppa:allegro/5.2
sudo apt-get update
```
Then, install Allegro and the necessary addons:
```bash
sudo apt-get install liballegro*5.2 liballegro*5-dev
```

**For Windows:**

There are several ways to get the Allegro binaries for Windows:
*   **Nuget:** The Allegro package on Nuget contains the official binaries in an easy-to-use format with MSVC.
*   **Allegro 5.2 binary packages:** The Allegro releases on GitHub also come with several binary distributions of Allegro for Windows.
*   **Allegro 5 dependencies:** The `allegro_winpkg` project on GitHub has releases that come with the Allegro dependencies pre-compiled for Windows.

Check the [Allegro wiki tutorial](https://liballeg.org/download.html#windows) for detailed installation instructions on Windows.

For other distributions or operating systems, consult the [official Allegro documentation](https://liballeg.org/download.html).

## 🚀 Compilation and Execution

The project includes VS Code tasks to facilitate compilation and execution. If you don't use VS Code, you can compile the `main.cpp` file manually from the terminal.

**Using VS Code tasks:**

1.  Open the project in VS Code.
2.  Open `main.cpp`.
3.  Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on macOS).
4.  Select the task:
    *   `Compile Active Allegro File`: Only compiles the file.
    *   `Compile and Run Active Allegro File`: Compiles and then runs the simulation.

**Manually from the terminal (Linux):**

Navigate to the project directory and run the following command to compile:
```bash
g++ main.cpp -o missionaries_cannibals -std=c++11 `pkg-config --cflags allegro-5 allegro_font-5 allegro_primitives-5` `pkg-config --libs allegro-5 allegro_font-5 allegro_primitives-5`
```
Then, to run the simulation:
```bash
./missionaries_cannibals
```

## 🎮 How to Play

1.  **Set Initial Numbers:**
    *   When you start, you will be on the configuration screen.
    *   Use the **UP ARROW** and **DOWN ARROW** keys to increase or decrease the number of selected missionaries or cannibals.
    *   Use the **LEFT ARROW** and **RIGHT ARROW** keys to switch between selecting missionaries and cannibals.
    *   Press **ENTER** to start the simulation with the set numbers.

2.  **Solution Visualization:**
    *   If a solution is found, the animation will begin to show the steps.
    *   Use the **UP ARROW** to increase the simulation speed (maximum 10x).
    *   Use the **DOWN ARROW** to decrease the simulation speed (minimum 0.5x).
    *   Press **R** to return to the main menu at any time.
    *   Press **SPACE** to restart the simulation with the same initial values.

3.  **End of Simulation:**
    *   If a solution is found, the message "SOLUTION FOUND!" and "Simulation Complete" will be displayed.
    *   The results are automatically saved in a `results.txt` file in the project directory.
    *   If no solution is found for the given values, "NO SOLUTION FOUND for these values." will be displayed.
    *   Press **ESC** at any time to exit the program.
    *   Press **R** to return to the main menu and try other values.

## 📜 Results File

When a successful solution is found, the program automatically generates a `results.txt` file containing:
*   The problem parameters (initial number of missionaries and cannibals)
*   The capacity of the boat
*   A detailed sequence of all the solution steps

**Example of file content:**
```
Solution for Missionaries: 3, Cannibals: 3
Boat capacity: 2
------------------------------------------
Step 0: Left(M:3, C:3) Right(M:0, C:0) Boat:Left
Step 1: Left(M:3, C:1) Right(M:0, C:2) Boat:Right
Step 2: Left(M:3, C:2) Right(M:0, C:1) Boat:Left
...
```
