/**\file			ui_lua.h
 * \author			Matt Zweig (thezweig@gmail.com)
 * \date			Created: Thursday, October 29, 2009
 * \date			Modified: Friday, November 14, 2009
 * \brief			Lua bridge for UI objects
 * \details
 */


#ifndef __H_UI_LUA
#define __H_UI_LUA

#ifdef __cplusplus
extern "C" {
#endif
#	include <lua.h>
#	include <lauxlib.h>
#	include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "ui/ui.h"

#define EPIAR_UI_TABLE "Epiar.UI"
#define EPIAR_UI "UI"

class UI_Lua {
	public:
		// Functions to communicate with Lua
		static void RegisterUI(lua_State *L);

		static Widget* checkWidget(lua_State *L, int index);

		// Functions to create new Widgets
		static int newWindow(lua_State *L);
		static int newFrame(lua_State *L);
		static int newButton(lua_State *L);
		static int newTextbox(lua_State *L);
		static int newTextarea(lua_State *L);
		static int newLabel(lua_State *L);
		static int newPicture(lua_State *L);
		static int newCheckbox(lua_State *L);
		static int newSlider(lua_State *L);
		static int newTabContainer(lua_State *L);
		static int newTab(lua_State *L);
		static int newDropdown(lua_State *L);
		static int newParagraph(lua_State *L);
		static int newNavMap(lua_State *L);

		// Dialogs
		static int newConfirm(lua_State *L);
		static int newAlert(lua_State *L);

		static int addWidget(lua_State *L);
		static int search(lua_State *L);

		// Fuctions to get Widget information
		static int IsChecked(lua_State *L);
		static int GetText(lua_State *L);
		static int GetEdges(lua_State *L);
		static int GetX(lua_State *L);
		static int GetY(lua_State *L);
		static int GetW(lua_State *L);
		static int GetH(lua_State *L);
		static int SetX(lua_State *L);
		static int SetY(lua_State *L);
		static int SetW(lua_State *L);
		static int SetH(lua_State *L);

		// Functions to manipulate Widgets
		static int move(lua_State *L);
		static int close(lua_State *L);
		static int AddCallback(lua_State *L);
		static int AddPosCallback(lua_State *L);
		static int add(lua_State *L);
		static int setFormButton(lua_State *L);
		static int AddCloseButton(lua_State *L);
		static int rotatePicture(lua_State *L);
		static int setText(lua_State *L);
		static int setPicture(lua_State *L);
		static int setPictureCenter(lua_State *L);
		static int setBackground(lua_State *L);
		static int setChecked(lua_State *L);
		static int setSliderValue(lua_State *L);
		static int AddOption(lua_State *L);
		static int getWorldPosition(lua_State *L);
		static int SetPannable(lua_State *L);
		static int SetZoomable(lua_State *L);

	private:
};
#endif /*__H_UI_LUA*/
