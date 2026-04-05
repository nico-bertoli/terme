# TERME

An engine for creating 2D terminal rendered games.<br>
Some usage examples can be found in the [Terme Examples Repo](https://github.com/nico-bertoli/terme_examples)

## Main features
- Cross platform (Linux/Windows)
- Framerate-independent simulation
- Collision system with OnCollisionEnter/Exit events
- Sorting layers
- Particle effects
- Backgrounds support

## Preview
Trailer available [here](https://www.youtube.com/watch?v=TqpGVI95oNU&ab_channel=Nicol%C3%B2Bertoli) <br><br>

<img src="screenshots/screen1.png" style="height:500px; max-width:100%;">
<img src="screenshots/screen2.png" style="height:250px; max-width:100%;">
<img src="screenshots/screen3.png" style="height:250px; max-width:100%;">

## Installation

Update your conanfile.py:
```python
    def requirements(self):
        self.requires("terme/1.0.0@local/stable")
    
```

Update your bash build file to download the package from GitHub:
```bash
if ! conan search terme/1.0.0@local/stable 2>/dev/null | grep -q "Package_ID"; then
    echo "Creating terme package from GitHub..."
    TMP_DIR=$(mktemp -d)
    git clone --depth 1 https://github.com/nico-bertoli/terme.git "$TMP_DIR/terme"
    conan create "$TMP_DIR/terme/terme" --name=terme --version=1.0.0 --user=local --channel=stable
    rm -rf "$TMP_DIR"
fi
```

Link the library in CMake:
```cmake
find_package(terme REQUIRED)

target_link_libraries(your_target PRIVATE terme::terme)
```

## Audio

Playback uses [miniaudio](https://github.com/mackron/miniaudio) (Unlicense / MIT-0).
