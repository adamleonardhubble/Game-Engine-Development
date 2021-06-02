# Game Engine Development
### Alfie Horton  ●   Adam Hubble   ●   Jack Moorin   ●   ~~Joseph Hill~~

- In Sandbox
  - Press the **Space** key to toggle fullscreen.
  - Press the **Escape** key to close the window.
  - Use the **W**, **A**, **S** and **D** keys to move the camera around the scene.
  - Hold the **Left Mouse Button** to rotate the camera.
  
  - This level shows the physics, as well as model loading, different types of textures, and the Lua Script prefabs (Spawning the green cube).
  - Framebuffers can be seen here as well.
    - Press **F** to cycle through the different frame buffer types.
    - Press **M** to toggle MSAA on and off.
      - Use the **Up** and **Down** keys to cycle through different MSAA levels when it is on.
    - Press **B** to toggle wireframe mode.
    - With select frame buffer types, press **N** to cycle through different kernel effects.

  - Performance Profiler
    - Expand the dropdown to view the profiler.
    - The current FPS can be viewed, as well as the average FPS over a selected number of frames.
    - All other dropdowns show timings of specific things in the engine.

- In the Level Editor
  - When it opens, maximize the window.
  - From the start menu, a new project can be given a name and created, or an existing one can be opened.
  - When a new project is made, a level is opened with a light and a camera.
    - The light has a point light component (using the point light diffuse and specular colours, and the objects position) ana a directional light component (using the directional light diffuse and specular colours, and the objects rotation).
    - Shadows in the scene are from the directional component, but also depend on the objects position.
  - All tabs can be moved around as preferred.
  - The **Scene View** tab shows the current level with a controllable camera.
    - Scroll to move the camera forward and back.
    - Hold the **Right Mouse Button** with the mouse over the view to move the camera up, down, left, and right.
    - Hold the **Left Mouse Button** while pressing **Alt** with the mouse over the view to rotate the camera.
  - The **Game View** tab shows the level from the view of the camera in the level.
  - The **Display Size** slider can be used to change the size of the display the cameras render to.
  - Objects in 3D layers in the level can be selected in the **Objects** dropdown in the **Editor** tab.
  - A selected objects name, layer, tag, and components can be seen and altered in the **Components** tab.
  - The current setup can be saved to be returned to with the **Save** button.
  - The current setup can be run in the engine using the **Run** button (a 3D object needs to be in the scene for this to work).
    - The setup is also saved when this is done.
    - **Scene View** stops rendering and objects/components can't be changed.
    - When running, the game plays like it would in sandbox. 
    - The mouse needs to be over the correct level view to control the camera.
    - Frame buffer effects can be seen like in sandbox.
    - The **Performance Profiler** tab appears when running to be seen like in sandbox.
    - Click the **Stop** button to stop running the game and return to the Level Editor as normal.
  - The **Add** tab contains buttons to add default objects, joints, and UI.
    - Joints need to be told which objects to join.
    - Objects being connected by joints need to have physics applied to them.
    - Joints and UI that have been added to the level have their own dropdown lists in the **Editor** tab (Text doesn't render unless running the game).
    - Custom object models can be added to the game in the folder: "levelEditor/assets/savedJSON/*Project Name*/models". These will show in the **Add** tab and the **Mesh** component **Model** dropdown menu.
  - Layers, tags, and levels in the game can be edited in the **Level Menu** tab.
    - Can be renamed, added, deleted.
    - Extra levels can be added and made in a single project the Level Editor, levels can't be changed in the game.
  - Objects can be given new components in the **Components** tab.
  - If giving an object physics:
    - It needs both a **Rigidbody** component and a **Collider** component.
    - Rigidbody **Bounciness**, **Friction** and **Roll Resistance** need to be set for it to work when running the game.
  - If giving an object a **Lua Script** component: 
    - The **?** button can be used to see what can be done with Lua Scripts.
    - A new Lua script can be made, or an existing one can be applied to the object.
    - The script can be opened from this component menu or from the folder "levelEditor/assets/savedJSON/*Project Name*/LuaScript".
  - Different texture types can be applied to an object with a material from the **Material** component dropdown (**FBOShader** is not a texture option).
    - Textures can be added to the game in the folder "levelEditor/assets/savedJSON/*Project Name*/textures". These will show in the texture selection dropdown menus.
  - The **New** and **Load** buttons both leave the current project and return to the start menu.
  - The **Physics** and **TeamFunkGame** projects were both used to test all aspects of the level editor.
    - **Physics** uses raycasting in a Lua Script and outputs to the console what is within a certain distance below **Sphere1** when running the game.

