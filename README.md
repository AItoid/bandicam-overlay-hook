### BandicamHook
Hijack Bandicam's overlay for rendering onto the screen with DirectX11.

### Setup
1. Compile the DLL with Visual Studio 2022.
2. Open Bandicam (admin, default), and select the third blue rectangle at the top ('Game Recording Mode.')
4. Select the tab 'FPS', and select 'Show FPS Overlay' to ensure the overlay is working for your game.
5. Load the module into the game.

### How It Works
Bandicam will load a trusted (signed) module into the game based on the architecture. If the game is 64-bit, Bandicam will load bdcam64.dll. If the game is 32-bit, Bandicam will load bdcam32.dll. These dlls are responsible for hooking present, and rendering the overlay from the present function in IDXGISwapChain::Present. To hook a function, you must have an input function to redirect code to. Because of this, we can 'hijack' Bandicam's redirect function that's a parameter in their hooking function, and thus getting our rendering hook to work. We hook both IDXGISwapChain::Present, and IDXGISwapChain::ResizeBuffers. Present is responsible for drawing, and ResizeBuffers is responsible for fixing the renderer when we re-size the screen.

![t](https://i.imgur.com/M6fjykS.png)

### Media
![f](https://i.imgur.com/hYuRRRY.png)
![s](https://i.imgur.com/ZbN7Xn5.png)

### Notes
- Bandicam's in-game overlay is beneath the Steam & NVIDIA Geforce Experience overlay.
- Only supports DirectX11.
