*This project has been created as part of the 42 curriculum by tkusama, mishimod.*

# cub3D

## Description

cub3D is a 3D graphical project inspired by the classic game Wolfenstein 3D. It uses raycasting (DDA algorithm) to render a first-person perspective view of a maze defined in a `.cub` configuration file. The player can move through the maze and look around in real time.

Built with the MinilibX graphics library and a custom C library (libft).

## Instructions

### Build

```bash
make        # Build the executable
make clean  # Remove object files
make fclean # Full cleanup (executable + libraries)
make re     # Rebuild from scratch
```

### Run

```bash
./cub3D <path_to_map.cub>
```

Example:

```bash
./cub3D maps/valid.cub
```

### Controls

| Key | Action |
|-----|--------|
| W / A / S / D | Move forward / left / backward / right |
| Left / Right Arrow | Rotate camera |
| ESC | Quit |

### Map file format (.cub)

```
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 220,100,0
C 0,150,255

111111
100001
10N001
100001
111111
```

- `NO/SO/WE/EA`: Wall texture paths (XPM format)
- `F/C`: Floor and ceiling RGB colors (0-255)
- Map: `1` = wall, `0` = floor, `N/S/E/W` = player spawn and direction

## Resources

- [Lode's Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html) — The primary reference for the DDA raycasting algorithm
- [Ray Casting Tutorial](https://www.permadi.com/tutorial/raycast/rayc1.html) — A beginner-friendly raycasting tutorial with step-by-step illustrations
- [Raycasting explained – Fortress Wolfenstein](https://www.swantiez.org/wolf/2014/08/16/raycasting-explained/) — The technique behind the Wolfenstein 3D engine
- [42 MinilibX documentation](https://harm-smits.github.io/42docs/libs/minilibx)
- [Piskel](https://www.piskelapp.com/) — Used to create sprite animations
