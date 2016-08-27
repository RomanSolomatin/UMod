// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

struct FVertex {
	int X;
	int Y;
	int U;
	int V;

	FVertex(int x, int y, int u, int v) {
		X = x;
		Y = y;
		U = u;
		V = v;
	}
};

struct FRuntimeCachedFont {
	UFont *FontObject;
	uint32 FontSize;
	FName TypeName;

	FSlateFontInfo SlateFont;

	FCharacterList *CharList;

	FRuntimeCachedFont(UFont *fnt, uint32 size, FName type) {
		FontObject = fnt;
		FontSize = size;
		TypeName = type;

		SlateFont = FSlateFontInfo(fnt, size, type);			
	}
};

#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_RIGHT 2

class URender2D
{
public:
	static void PushMatrix(FMatrix mat);
	static void PopMatrix();

	static void SetContext(UCanvas *c);
	static void ExitContext();

	//Set draw color for all upcoming draw operations
	static void SetColor(FColor col);
	//Set draw texture for all compatible upcoming draw operations
	static void SetTexture(uint32 id);
	static void ResetTexture();
	//Set text font
	static void SetFont(uint32 id);
	//Loads font from path
	static uint32 LoadFont(FString path, uint32 Size, FName ComposeType = NAME_None);
	//Unloads font
	static void UnloadFont(uint32 id);
	//Loads texture from path
	static uint32 LoadTexture(FString path);
	//Unloads texture
	static void UnloadTexture(uint32 id);
	//Draws a rectangle
	static void DrawRect(float x, float y, float w, float h);
	//Draw an outline rectangle
	static void DrawOutlineRect(float x, float y, float w, float h, float stroke);
	//Draw polygon with vertex array (NOTE : need special code...)
	static void DrawPoly(TArray<FVertex> vertices);
	//Draw a circle with given rayon (NOTE : need special code...)
	static void DrawCircle(float x, float y, int r);
	//Draw a rounded rectangle (NOTE : need special code...)
	static void DrawRoundedRect(float x, float y, float w, float h, int rayon);
	//Set the scissoring rectangle
	static void SetScissorRect(int x, int y, int w, int h);
	//Draws a string
	static void DrawText(FString str, float x, float y, uint8 align);
	//Return the size of a text
	static void GetTextSize(FString str, float& w, float& h);
	//Set the font scale
	static void SetFontScale(float sx, float sy);
	//Draw a line starting from (x, y) and ending to (x1, y1) using stroke
	static void DrawLine(float x, float y, float x1, float y1, float stroke);

	//Check if the context exists (used by Lua)
	static bool CheckContext();
private:
	static void FindNextTextureMapSlot();
	static void FindNextFontMapSlot();
};
