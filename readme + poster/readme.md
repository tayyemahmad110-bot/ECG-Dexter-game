# Dexter's Dimensional Disaster
**Course:** Computer Graphics (OpenGL) — 2026–2027  
**Theme:** Cartoon Network (*Dexter's Laboratory*)  
**Group:** Internet of Things (Wednesday, 16:00–18:00)

---

## 1. Game Storyline: "Dee Dee's Dimensional Disaster"

### Premise
During one of Dexter's late-night experiments in his secret underground laboratory, his sister Dee Dee sneaks in and presses the forbidden **"Big Red Button."** The resulting dimensional surge overloads the lab's **Core Generator**, scattering essential blueprints and Core Power Cells throughout the facility and out into the suburban neighborhood above. Worse, the shockwave corrupts the laboratory's automated security bots, turning them hostile toward Dexter.

### Part 1 — The 2D Teaser (Escape the Glitched Lab)
Caught deep inside Sector 4 when the lockdown initiates, Dexter grabs his portable laser blaster and sprints toward the **Main Blast Door**. In this 2D side-scrolling teaser, Dexter must leap over displaced lab crates, shoot down tumbling security robots, and recover scattered **Core Power Cells** to stabilize his suit's energy shields before reaching the exit.

### Part 2 — The 3D RPG Adventure (Full Implementation)
Having escaped the inner sector, Dexter must explore two interconnected 3D environments to fully reboot the Core Generator:
* **Scene 1 (Indoor):** The sprawling, multi-level metallic interior of Dexter's Laboratory.
* **Scene 2 (Outdoor):** The brightly colored suburban neighborhood outside Dexter's house.

#### Sequential 3D Questline (5 Tasks):
1. **Equip the Trusty Wrench:** Locate and pick up Dexter's multi-tool wrench inside the main lab (attaches to the player via the Scene Graph hierarchy).
2. **Recover the Blast Door Blueprints:** Search the server room for the scattered schematic needed to override the upper-level security lock.
3. **Retrieve the Core Power Cell (Outdoor):** Exit into the suburban backyard and climb toward the treehouse area to recover the stolen Core Power Cell.
4. **Neutralize the Glitched Laser Grid:** Return to the lab entrance and use the Power Cell from the inventory to deactivate the malfunctioning laser barrier.
5. **Reboot the Main Computer:** Access the central terminal in the Core Chamber to restore power and save the laboratory.

---

## 2. Part 1: 2D Teaser Technical Implementation

Built in **Modern OpenGL** (Shaders, VAO, VBO, IBO, and GLM matrix transformations) starting from our 2D transformations lab framework, the teaser implements all core rubric requirements:

### Controls
* **`A` / `D`**: Move Dexter left and right (with dynamic side-scrolling camera tracking)
* **`W`**: Jump (gravity & kinematic velocity simulation)
* **`SPACE`**: Fire laser blaster
* **`R`**: Restart level after Victory or Game Over
* **`ESC`**: Exit application

### Rubric Feature Breakdown
1. **Multiple Complex Shapes / Objects Built from Primitives:**
   * **Dexter:** Assembled dynamically from 6 distinct scaled and colored primitives (boots, white lab coat, head, signature orange hair, black glasses, and purple lab glove).
   * **Glitched Security Robots:** Built from a metallic chassis, glowing red visor, and top antenna.
   * **Lab Blast Door (Finish Line):** Constructed from a dark steel outer frame, sliding metallic doors, vertical center seam, green exit beacon, and security keypad.
   * **Laboratory Environment:** Scrolling background featuring server pillars, glowing cyan/green monitors, overhead pipes, metallic flooring, and wooden/metal obstacle crates.

2. **Two Types of Complex Animations:**
   * **Tumbling Security Robots:** Combine horizontal world translation with continuous rotation around their own local center ($\mathbf{M} = \mathbf{T} \cdot \mathbf{R}$).
   * **Floating Core Power Cells:** Combine vertical sinusoidal oscillation ($y(t) = y_0 + A\sin(\omega t)$), continuous local-axis rotation, and periodic scale pulsing.

3. **Two Original Algorithms / Mathematical Approaches:**
   * **Kinematic Jump & Platform Physics:** Vertical displacement governed by initial jump impulse ($v_y$), constant downward gravitational acceleration ($g$), and surface height resolution for both the lab floor and elevated crates.
   * **AABB Collision & Life/Damage System:** Axis-Aligned Bounding Box checks detect intersections between lasers, enemies, crates, and collectibles. Taking damage triggers a timed invulnerability window ($t_{\text{invuln}}$) and updates the top-left 2D HUD health bar and console telemetry.