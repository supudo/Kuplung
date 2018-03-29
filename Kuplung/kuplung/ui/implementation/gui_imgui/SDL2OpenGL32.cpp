//
//  SDL2OpenGL32.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/implementation/gui_imgui/SDL2OpenGL32.hpp"

static const char* ImGui_Implementation_GetClipboardText(void*) {
	return SDL_GetClipboardText();
}

static void ImGui_Implementation_SetClipboardText(void*, const char* text) {
	SDL_SetClipboardText(text);
}

void SDL2OpenGL32::init(SDL_Window *window) {
    this->sdlWindow = window;
}

void SDL2OpenGL32::ImGui_Implementation_RenderDrawData(ImDrawData* draw_data) {
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
	{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
	{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
	{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(this->gui_ShaderHandle);
	glUniform1i(this->gui_AttribLocationTex, 0);
	glUniformMatrix4fv(this->gui_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindSampler(0, 0); // Rely on combined texture/sampler state.

						 // Recreate the VAO every time 
						 // (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
	GLuint vao_handle = 0;
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, this->gui_VboHandle);
	glEnableVertexAttribArray(this->gui_AttribLocationPosition);
	glEnableVertexAttribArray(this->gui_AttribLocationUV);
	glEnableVertexAttribArray(this->gui_AttribLocationColor);
	glVertexAttribPointer(this->gui_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(this->gui_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(this->gui_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, this->gui_VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->gui_ElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}
	glDeleteVertexArrays(1, &vao_handle);

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindSampler(0, last_sampler);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool SDL2OpenGL32::ImGui_Implementation_Init() {
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)

    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

	io.SetClipboardTextFn = ImGui_Implementation_SetClipboardText;
	io.GetClipboardTextFn = ImGui_Implementation_GetClipboardText;

	this->gui_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	this->gui_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	this->gui_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	this->gui_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	this->gui_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	this->gui_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	this->gui_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	io.ClipboardUserData = NULL;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(this->sdlWindow, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    return true;
}

void SDL2OpenGL32::ImGui_Implementation_Shutdown() {
	// Destroy SDL mouse cursors
	for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
		SDL_FreeCursor(this->gui_MouseCursors[cursor_n]);
	memset(this->gui_MouseCursors, 0, sizeof(this->gui_MouseCursors));

	// Destroy OpenGL objects
	this->ImGui_Implementation_InvalidateDeviceObjects();
}

void SDL2OpenGL32::ImGui_Implementation_NewFrame() {
	if (!this->gui_FontTexture)
		this->ImGui_Implementation_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	SDL_GetWindowSize(this->sdlWindow, &w, &h);
	SDL_GL_GetDrawableSize(this->sdlWindow, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	Uint64 current_time = SDL_GetPerformanceCounter();
	io.DeltaTime = this->gui_Time > 0 ? (float)((double)(current_time - this->gui_Time) / frequency) : (float)(1.0f / 60.0f);
	this->gui_Time = current_time;

	// Setup mouse inputs (we already got mouse wheel, keyboard keys & characters from our event handler)
	int mx, my;
	Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseDown[0] = this->gui_MousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = this->gui_MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = this->gui_MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	this->gui_MousePressed[0] = this->gui_MousePressed[1] = this->gui_MousePressed[2] = false;

	// We need to use SDL_CaptureMouse() to easily retrieve mouse coordinates outside of the client area. This is only supported from SDL 2.0.4 (released Jan 2016)
#if (SDL_MAJOR_VERSION >= 2) && (SDL_MINOR_VERSION >= 0) && (SDL_PATCHLEVEL >= 4)   
	if ((SDL_GetWindowFlags(this->sdlWindow) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_MOUSE_CAPTURE)) != 0)
		io.MousePos = ImVec2((float)mx, (float)my);
	bool any_mouse_button_down = false;
	for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
		any_mouse_button_down |= io.MouseDown[n];
	if (any_mouse_button_down && (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_CAPTURE) == 0)
		SDL_CaptureMouse(SDL_TRUE);
	if (!any_mouse_button_down && (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_CAPTURE) != 0)
		SDL_CaptureMouse(SDL_FALSE);
#else
	if ((SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_INPUT_FOCUS) != 0)
		io.MousePos = ImVec2((float)mx, (float)my);
#endif

	// Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
	if ((io.ConfigFlags & ImGuiConfigFlags_NoSetMouseCursor) == 0)
	{
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
		{
			SDL_ShowCursor(0);
		}
		else
		{
			SDL_SetCursor(this->gui_MouseCursors[cursor] ? this->gui_MouseCursors[cursor] : this->gui_MouseCursors[ImGuiMouseCursor_Arrow]);
			SDL_ShowCursor(1);
		}
	}

	// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();
}

bool SDL2OpenGL32::ImGui_Implementation_ProcessEvent(SDL_Event* event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
        case SDL_MOUSEWHEEL: {
			if (event->wheel.x > 0) io.MouseWheelH += 1;
			if (event->wheel.x < 0) io.MouseWheelH -= 1;
			if (event->wheel.y > 0) io.MouseWheel += 1;
			if (event->wheel.y < 0) io.MouseWheel -= 1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN: {
			if (event->button.button == SDL_BUTTON_LEFT) this->gui_MousePressed[0] = true;
			if (event->button.button == SDL_BUTTON_RIGHT) this->gui_MousePressed[1] = true;
			if (event->button.button == SDL_BUTTON_MIDDLE) this->gui_MousePressed[2] = true;
            return true;
        }
        case SDL_TEXTINPUT: {
			io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
			int key = event->key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (event->type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
            return true;
        }
    }
    return false;
}

void SDL2OpenGL32::ImGui_Implementation_InvalidateDeviceObjects() {
    if (this->gui_VaoHandle)
        glDeleteVertexArrays(1, &this->gui_VaoHandle);
    if (this->gui_VboHandle)
        glDeleteBuffers(1, &this->gui_VboHandle);
    if (this->gui_ElementsHandle)
        glDeleteBuffers(1, &this->gui_ElementsHandle);
    this->gui_VaoHandle = this->gui_VboHandle = this->gui_ElementsHandle = 0;

    glDetachShader(this->gui_ShaderHandle, this->gui_VertHandle);
    glDeleteShader(this->gui_VertHandle);
    this->gui_VertHandle = 0;

    glDetachShader(this->gui_ShaderHandle, this->gui_FragHandle);
    glDeleteShader(this->gui_FragHandle);
    this->gui_FragHandle = 0;

    glDeleteProgram(this->gui_ShaderHandle);
    this->gui_ShaderHandle = 0;

    if (this->gui_FontTexture) {
        glDeleteTextures(1, &this->gui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        this->gui_FontTexture = 0;
    }
}

bool SDL2OpenGL32::ImGui_Implementation_CreateDeviceObjects() {
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	const GLchar* vertex_shader_with_version[2] = { this->gui_GlslVersion, vertex_shader };
	const GLchar* fragment_shader_with_version[2] = { this->gui_GlslVersion, fragment_shader };

	this->gui_ShaderHandle = glCreateProgram();
	this->gui_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	this->gui_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->gui_VertHandle, 2, vertex_shader_with_version, NULL);
	glShaderSource(this->gui_FragHandle, 2, fragment_shader_with_version, NULL);
	glCompileShader(this->gui_VertHandle);
	glCompileShader(this->gui_FragHandle);
	glAttachShader(this->gui_ShaderHandle, this->gui_VertHandle);
	glAttachShader(this->gui_ShaderHandle, this->gui_FragHandle);
	glLinkProgram(this->gui_ShaderHandle);

	this->gui_AttribLocationTex = glGetUniformLocation(this->gui_ShaderHandle, "Texture");
	this->gui_AttribLocationProjMtx = glGetUniformLocation(this->gui_ShaderHandle, "ProjMtx");
	this->gui_AttribLocationPosition = glGetAttribLocation(this->gui_ShaderHandle, "Position");
	this->gui_AttribLocationUV = glGetAttribLocation(this->gui_ShaderHandle, "UV");
	this->gui_AttribLocationColor = glGetAttribLocation(this->gui_ShaderHandle, "Color");

	glGenBuffers(1, &this->gui_VboHandle);
	glGenBuffers(1, &this->gui_ElementsHandle);

	this->ImGui_Implementation_CreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}

void SDL2OpenGL32::ImGui_Implementation_CreateFontsTexture() {
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

															  // Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &this->gui_FontTexture);
	glBindTexture(GL_TEXTURE_2D, this->gui_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)this->gui_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}
