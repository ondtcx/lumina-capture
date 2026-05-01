---
name: Lumina Capture
colors:
  surface: '#0b1326'
  surface-dim: '#0b1326'
  surface-bright: '#31394d'
  surface-container-lowest: '#060e20'
  surface-container-low: '#131b2e'
  surface-container: '#171f33'
  surface-container-high: '#222a3d'
  surface-container-highest: '#2d3449'
  on-surface: '#dae2fd'
  on-surface-variant: '#cbc3d7'
  inverse-surface: '#dae2fd'
  inverse-on-surface: '#283044'
  outline: '#958ea0'
  outline-variant: '#494454'
  surface-tint: '#d0bcff'
  primary: '#d0bcff'
  on-primary: '#3c0091'
  primary-container: '#a078ff'
  on-primary-container: '#340080'
  inverse-primary: '#6d3bd7'
  secondary: '#adc6ff'
  on-secondary: '#002e6a'
  secondary-container: '#0566d9'
  on-secondary-container: '#e6ecff'
  tertiary: '#fbabff'
  on-tertiary: '#580065'
  tertiary-container: '#e14ef6'
  on-tertiary-container: '#4d0059'
  error: '#ffb4ab'
  on-error: '#690005'
  error-container: '#93000a'
  on-error-container: '#ffdad6'
  primary-fixed: '#e9ddff'
  primary-fixed-dim: '#d0bcff'
  on-primary-fixed: '#23005c'
  on-primary-fixed-variant: '#5516be'
  secondary-fixed: '#d8e2ff'
  secondary-fixed-dim: '#adc6ff'
  on-secondary-fixed: '#001a42'
  on-secondary-fixed-variant: '#004395'
  tertiary-fixed: '#ffd6fd'
  tertiary-fixed-dim: '#fbabff'
  on-tertiary-fixed: '#36003e'
  on-tertiary-fixed-variant: '#7c008e'
  background: '#0b1326'
  on-background: '#dae2fd'
  surface-variant: '#2d3449'
typography:
  h1:
    fontFamily: Space Grotesk
    fontSize: 32px
    fontWeight: '700'
    lineHeight: '1.2'
    letterSpacing: -0.02em
  h2:
    fontFamily: Space Grotesk
    fontSize: 24px
    fontWeight: '600'
    lineHeight: '1.3'
  body-lg:
    fontFamily: Inter
    fontSize: 18px
    fontWeight: '400'
    lineHeight: '1.5'
  body-md:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: '400'
    lineHeight: '1.5'
  label-caps:
    fontFamily: Space Grotesk
    fontSize: 12px
    fontWeight: '700'
    lineHeight: '1'
    letterSpacing: 0.05em
  mono:
    fontFamily: Space Grotesk
    fontSize: 13px
    fontWeight: '500'
    lineHeight: '1.4'
rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px
spacing:
  unit: 4px
  xs: 4px
  sm: 8px
  md: 16px
  lg: 24px
  xl: 40px
  gutter: 16px
  margin: 24px
---

## Brand & Style

This design system is built for a high-performance developer utility where technical precision meets creative energy. The brand personality is **Precise**, **Vibrant**, and **Unobtrusive**. It targets developers and power users who value C++-level performance but desire a modern, polished interface.

The visual style leverages **Glassmorphic Minimalism**. By combining a strict minimalist layout with translucent, frosted layers and vibrant background blurs, the design remains functional while feeling cutting-edge. The UI stays out of the way of the user's content, appearing as a lightweight, floating toolkit rather than a heavy desktop application.

## Colors

The palette is anchored in a deep, "Space" dark mode to provide maximum contrast for captured screen content. 

- **Primary (Vivid Purple):** Used for main actions and active states.
- **Secondary (Bright Blue):** Used for selection bounds and technical indicators.
- **Tertiary (Electric Pink):** Used sparingly for highlights, notifications, and recording indicators.
- **Gradients:** Use linear gradients (45-degree angle) transitioning from Primary to Tertiary for high-impact areas like hero buttons or capture progress bars.
- **Glass Effects:** Use the background color at 60% opacity with a 20px backdrop blur for panels and floating toolbars.

## Typography

The typography strategy balances technical geometry with high-speed legibility.

- **Space Grotesk** is used for headlines and technical labels (like resolution counters or file sizes) to lean into the developer-tool aesthetic. Its geometric nature reflects the precision of a C++ application.
- **Inter** is used for all body copy and settings menus to ensure maximum readability during complex configurations.
- **Hierarchy:** Use heavy weight caps for labels to differentiate between "content" and "control." Use the Mono variant for displaying coordinate data (X/Y) or hex codes.

## Layout & Spacing

This design system utilizes a **No Grid / Floating UI** philosophy. Since the application focuses on screen real estate, the UI consists of floating toolbars and context-sensitive overlays rather than a fixed-width container.

- **The 4px Rhythm:** All padding, margins, and component heights must be multiples of 4px.
- **Safe Zones:** Keep a 24px margin from the edge of the screen for all persistent UI elements.
- **Contextual Grouping:** Use 8px spacing between related items (e.g., capture mode icons) and 16px between different functional groups (e.g., capture tools vs. settings).

## Elevation & Depth

Depth is established through **Backdrop Blurs** and **Tinted Ambient Shadows**.

1.  **Level 0 (Base):** The desktop or the content being captured.
2.  **Level 1 (Toolbars):** 60% opacity surface color with a 16px backdrop blur. A subtle 1px inner border (#FFFFFF at 10% opacity) provides a "glass" edge.
3.  **Level 2 (Popovers/Menus):** 80% opacity surface. Use a diffused shadow: `0 10px 25px -5px rgba(0, 0, 0, 0.5)`. 
4.  **Active Focus:** When a capture area is active, the surrounding "dead space" is filled with a 40% black overlay to focus the user's eye.

## Shapes

The shape language is **Refined and Intentional**. 

- **Standard Elements:** Use a 0.5rem (8px) radius for buttons and input fields to maintain a professional look.
- **Main Containers:** Large floating panels use a 1rem (16px) radius to feel softer and more modern.
- **Selection Bounds:** The capture crosshairs and area selectors should use sharp corners (0px) or very minimal rounding (2px) to signify mathematical precision.

## Components

- **Buttons:** 
    - *Primary:* Gradient fill (Purple to Pink), white text, 8px radius. 
    - *Ghost:* No fill, 1px white border at 20% opacity.
- **Toolbars:** Floating horizontal bars with glassmorphism effects. Icons should be 20px, stroke-based (1.5pt thickness).
- **Chips:** Used for "tags" like resolution (e.g., "1080p") or file format. Small 4px radius, dark background with a 1px primary-colored border.
- **Capture Area Selector:** A 2px thick Blue stroke with Tertiary-colored "handles" at the corners for resizing.
- **Input Fields:** Darker than the surface color, minimal bottom-border focus state in Primary purple.
- **Recording Indicator:** A Tertiary (Pink) pulsating dot next to a Space Grotesk timer.
- **Lists:** Clean, no dividers. Use a subtle hover state with a 5% white overlay and 4px rounded corners for the selection highlight.