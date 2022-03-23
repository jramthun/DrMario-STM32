# DrMario-STM32
Implementation of Dr. Mario for Purdue ECE 362 Mini-project (feat. STM32F091RCTx)

## Getting Started
Software Requirements: Ac6 System Workbench, STM32F0 Std Peripheral Driver, and [Img2Cpp](https://javl.github.io/image2cpp/) for image to array conversion
Hardware Requirements: STM32F091RCTx and Purdue ECE Master Lab Kit (for relevant hardware)

## Custom Work
Redesigned TFT-LCD driver to accommodate "non-standard" requirements such as:
- Custom font sizing (standard is 12px or 16px)
- Custom image loading (standard driver did not support)
- Fast initialization of display (standard stops at blank screen)
- Simultaneous, repeating audio (i.e., audio loops without stopping the game)

## Team Work
Elijah Klien: starting up Audio via DMA
Santiago Garcia: handling the layout and internal game controller

## Acknowledgements
- ECE 362 Staff (thanks Rick!)
- [Img2Cpp](https://javl.github.io/image2cpp/)
- Sprites: [A.J. Nitro](http://www.mariouniverse.com/wp-content/img/sprites/nes/drm/misc-3.png)
