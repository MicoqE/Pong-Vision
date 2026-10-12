# Pong Vision 🎮

**A classic Pong game controlled by computer vision, built with C++, OpenCV, and a webcam.**

Pong Vision explores the combination of classic arcade gameplay and real-time computer vision. Instead of controlling paddles with a keyboard, players use colored objects detected by a webcam.

The project is developed as a hands-on experiment in C++, image processing, and interactive systems.

## Features

- 🎮 Two-player Pong gameplay.
- 📷 Real-time webcam input using OpenCV.
- 🟢 Green object detection for the left paddle.
- 🔵 Blue object detection for the right paddle.
- 🏓 Ball movement, paddle collisions, and score tracking.
- 🖥️ Game window rendered with OpenCV.
- 🏠 **Start menu** displayed when the application launches.
- 🎯 **Adjustable winning score** before starting a match.
- ⏱️ **Countdown** before gameplay begins.
- 🏆 **End-of-game screen** displaying the winner and final score.
- 🔄 Option to restart a match or return to the menu.
- ⌨️ Keyboard controls for menu navigation and game management.
- ⚙️ MSVC build configuration for Visual Studio Code.

## Demo

*Screenshots and gameplay videos will be added here.*

## Requirements

- Windows 10 or Windows 11.
- Visual Studio Build Tools with the **Desktop development with C++** workload.
- MSVC x64 compiler.
- OpenCV 5.0 for Windows, with MSVC-compatible libraries.
- A webcam.
- Two colored objects or LEDs: green and blue.
- Git and Visual Studio Code (recommended).

## Project Structure

```text
Pong-Vision/
├── .vscode/
│   ├── tasks.json
│   └── build-pong.bat
├── main.cpp
├── README.md
├── .gitignore
└── opencv/                 # Local dependency; not tracked by Git
```

The `opencv/` directory is a local dependency and is intentionally excluded from the repository to avoid committing large third-party binaries.

## Installation

### 1. Clone the repository

```bash
git clone <YOUR_REPOSITORY_URL>
cd Pong-Vision
```

Replace `<YOUR_REPOSITORY_URL>` with the URL of your GitHub repository.

### 2. Install Visual Studio Build Tools

Install Visual Studio Build Tools and select the **Desktop development with C++** workload.

Make sure the MSVC x64 compiler and a Windows SDK are installed.

Official download:

https://visualstudio.microsoft.com/visual-cpp-build-tools/

### 3. Install OpenCV

Download OpenCV for Windows from the official website:

https://opencv.org/releases/

Extract the package into the project directory so the build configuration can find the following paths:

```text
opencv/
└── build/
    ├── include/
    └── x64/
        └── vc16/
            ├── bin/
            └── lib/
```

The current build script expects these files:

- `opencv/build/include/`
- `opencv/build/x64/vc16/lib/opencv_world500.lib`
- `opencv/build/x64/vc16/bin/opencv_world500.dll`

The library and DLL names correspond to the OpenCV 5.0 package used during development. If you install a different OpenCV version or package layout, update the build configuration accordingly.

**Important:** The project uses MSVC-compatible OpenCV binaries. MinGW-built libraries are not interchangeable with these MSVC libraries.

### 4. Build and run in Visual Studio Code

Open the project folder in Visual Studio Code.

The repository includes build tasks in `.vscode/tasks.json` and a Windows batch script in `.vscode/build-pong.bat`.

Use:

```text
Ctrl + Shift + B
```

to run the default build task.

If the task is configured to compile and launch the game, it should build `Pong.exe` and then start it with the OpenCV DLL directory available at runtime.

If compilation fails, check the MSVC installation, OpenCV paths, and library compatibility.

## How to Play

The webcam captures images continuously. OpenCV processes the images to detect the configured colors. The detected vertical positions are used to move the paddles.

| Object | Player |
|---|---|
| Green object or LED | Left paddle |
| Blue object or LED | Right paddle |

Keyboard controls depend on the current game implementation.

## Menu and Controls

When the application starts, the main menu is displayed before gameplay begins.

From the menu, players can select the winning score and start a match. A countdown prepares both players before the ball starts moving.

| Input | Action |
|---|---|
| Space | Start a match or restart after the game ends |
| Left / Right arrows | Decrease or increase the winning score in the menu |
| M | Return to the main menu |
| Esc | Quit the game |

The first player to reach the selected winning score wins the match.

## Improving Detection

For more reliable results:

- Use bright green and blue objects or LEDs.
- Place the objects against a contrasting background.
- Avoid reflections and excessive ambient light.
- Keep the objects within the camera's field of view.
- Adjust the HSV thresholds in the source code when necessary.

LEDs can improve color consistency, but camera exposure, brightness, and reflections can still affect detection.

## Troubleshooting

### The webcam does not open

- Check Windows camera permissions.
- Close other applications using the webcam.
- Verify the camera index in the source code.

### OpenCV DLLs are missing

Make sure `opencv_world500.dll` is present in the configured DLL directory and that the launch task adds this directory to `PATH`.

### The project does not compile

Check that:

- MSVC is installed and available.
- The OpenCV include directory exists.
- The `.lib` file matches the installed OpenCV version.
- The project is using compatible x64 libraries.
- The build script points to the correct Visual Studio and OpenCV installations.

### Color detection is unreliable

Improve lighting, use a contrasting background, and adjust the color thresholds. Automatic camera exposure may also affect the results.

## Technologies

- **C++17**
- **OpenCV 5.0**
- **MSVC**
- **Visual Studio Code**
- **Git and GitHub**

## Project Status

Personal learning project — under development.

The goal is to explore computer vision, real-time image processing, and interactive game development using C++.

## License

This project is distributed under the MIT License. See the [MIT License](LICENSE) file at the root of the repository.

---

**Pong Vision** — Bringing computer vision into a classic arcade game.
