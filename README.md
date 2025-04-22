# Formation Creation And Coordinated Movement

Research on the creation of certain basic unit formations as seen in RTS games. This project also makes use of RTS style selection and input.

This project was made in Unreal Engine 5.2.1 in C++ (only using blueprints to assign data to the C++ Classes);

## Objective

Implementation of basic RTS style Formations and movement by moving selected units to calculated positions depending on the amount of actors. 
The goal I wanted to achieve was to create formations and some coordinated movement. 
In regards of grouping I decided to keep all units as standalone actors, this way you can rearange the actors in more complex shapes without needing to assign groups.
The project handles the formations by the size of the selected group and allows you to orient the formations in different directions.
Controls are handled similarly to RTS games due to it fitting best to the project.

## Project info:

### Installation Guide

1. Download Unreal Engine 5.2
2. Download the zip file
3. Extract the files and go to the .uproject file
4. Right click and Generate Visual Studio files (you might need to look in extra options)
5. Open the .uproject file and compile the code
6. You should now be able to run it

### Controls

- "WASD" basic up down left right movement
- "LMB" selecting unit (Hold and drag for box select. Clicking on the ground will deselect all)
- "RMB" move selected units to target location (with the current selected formation type)
- "RMB + drag" will orient the formations in the direction you dragged 
- "QE" rotate camera yaw by 30 degrees left and right respectively
- "MMB + drag" freelook for camera
- "C" Current Formation (takes relative position of all selected units and moves them in that shape)
- "I" V-Formation Movement
- "K" Spearhead-Formation Movement
- "O" Circle-Formation Movement
- "L" Line-Formation Movement (horizontal)
- "P" Pole-Formation Movement (vertical)

## Inplementation

### Game Engine

Unreal Engine was my choice because it allowed me to code in C++ and has a good base movement ai with navigation meshes. This also allowed me to become more familiar with unreal coding.
Choosing an engine like unity is also a good option as it allows for faster development but it is an engine I am less familiar with.

### Selection

Selection is done by left clicking on actors to select and deselect. when they are highlighted in green they are selected.
You can also select by holding left click and dragging. That results in a box selection and it will preview the actors you will select and deselect by changing the before mentioned highlight.
(note: the box selection only activates when you release the selection)

## Formations

### Current

This is the formation type that allows you to make use of the current arangement of actors, allowing you to make more complex stuctures or even drawings.
It works by calculating the average position of the selected group and calculating the relative positions before rotating (if you right click and drag).
This doesn't store the relative positions so if you retarget the formation during movement the formation will deform.

![CurrentFlormation](https://github.com/Howest-DAE-GD/gpp-researchtopic-SebastianMaxG/assets/64198670/8e9ece15-530d-483c-8574-183f21739e35)

### V / Spearhead

These formation types are V shaped formations by alternating the positions left and right of the selected target point. 
The V formation takes the selected point and moves the actors away from the point in the chosen direction creating a v shape.
The Spearhead formation takes the selected point and moves the actors away from the point in the inverse chosen direction creating an inverse v shape like a spearhead or arrow point.

![V-SpearFormation](https://github.com/Howest-DAE-GD/gpp-researchtopic-SebastianMaxG/assets/64198670/e2911d11-4bc1-4ebb-aaeb-c82167bdb135)

### Pole / Line 

These formation types order units linearly in a vertical line (pole) or horizontal line (line) respectively.
Here you can also see that the formations line up with the camera rotation.

![PoleFormation](https://github.com/Howest-DAE-GD/gpp-researchtopic-SebastianMaxG/assets/64198670/592d05ce-d3cc-415d-97c4-4a6ef9d20e4d)

### O / Circle

The O or circle formations uses the selected point as a centerpoint and calculates the radius by using a set radial distance between actors.
This makes sure the actors don't clump together and remain spaced out.

![OFormation](https://github.com/Howest-DAE-GD/gpp-researchtopic-SebastianMaxG/assets/64198670/62841ed9-687e-4edc-a3ee-b44d74753ea5)

## Movement

Since I just set the target positions of all the actors they move independently and can dodge obstacles more easily. 
For future reference I came to the conclusion that it would have been better if I interpolated the entire formation during movement.
This would help the actors stay in formation better while in movement and may cause the actors to collide less with eachother.
Or even by making use of steering circles to make sure the formation doesn't compress when moving around obstacles.

![DodgingPoles](https://github.com/Howest-DAE-GD/gpp-researchtopic-SebastianMaxG/assets/64198670/37edc98a-aa93-49ca-9e83-3b8f1fa7e340)

## Sources

- [RTS Tutorial - Rogue Entity](https://www.youtube.com/playlist?list=PLGEQZOX6SqiECSQflW6ag8brSs-qrggzh)
- [EezehDev AI-Formations](https://github.com/EezehDev/AI-Formations)

### Similar Topics

- [Christensen_Eiserloh_Thesis](https://andrewtc.dev/docs/Christensen_Eiserloh_Thesis.pdf)
- [Techniques_for_Formation_Movement_Using_Steering_Circles](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter21_Techniques_for_Formation_Movement_Using_Steering_Circles.pdf)

-Leander Flossie

