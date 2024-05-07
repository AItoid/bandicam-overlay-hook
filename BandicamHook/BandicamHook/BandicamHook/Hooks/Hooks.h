#pragma once
#include "../Includes.h"

namespace Hooks
{
	typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall* ResizeBuffers) (IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	inline bool init = false;
	inline Present oPresent;
	inline ResizeBuffers oResizeBuffers;
	inline HWND window = NULL;
	inline WNDPROC oWndProc;
	inline ID3D11Device* pDevice = NULL;
	inline ID3D11DeviceContext* pContext = NULL;
	inline ID3D11RenderTargetView* mainRenderTargetView;

	inline void InitImGui()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
	}

	inline LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	inline HRESULT __stdcall ResizeBuffersHk(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		if (mainRenderTargetView)
		{
			pContext->OMSetRenderTargets(0, 0, 0);
			mainRenderTargetView->Release();
		}

		HRESULT Result = veh::CallOriginal<HRESULT>(oResizeBuffers, SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

		ID3D11Texture2D* Buffer = nullptr;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Buffer);

		pDevice->CreateRenderTargetView(Buffer, NULL, &mainRenderTargetView);
		Buffer->Release();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

		D3D11_VIEWPORT Vp{};
		Vp.Width = static_cast<float>(Width);
		Vp.Height = static_cast<float>(Height);
		Vp.MinDepth = 0.f;
		Vp.MaxDepth = 1.f;
		Vp.TopLeftX = 0;
		Vp.TopLeftY = 0;

		pContext->RSSetViewports(1, &Vp);
		return Result;
	}

	inline HRESULT __stdcall PresentHk(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!init)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);

				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);

				window = sd.OutputWindow;
				ID3D11Texture2D* pBackBuffer;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
				pBackBuffer->Release();
				oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

				InitImGui();
				init = true;
			}
			else
			{
				return veh::CallOriginal<HRESULT>(oPresent, pSwapChain, SyncInterval, Flags);
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return veh::CallOriginal<HRESULT>(oPresent, pSwapChain, SyncInterval, Flags);
	}
}