// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "Render2D.h"
#include "CanvasScissorRect.h"
#include "UModGameInstance.h"

//Statics here as linker is unable to link correctly with static vars inside H file
static FVector2D FontScale;

static FColor CurColor;
static uint32 CurFont;
static uint32 CurTexture;
static bool NoTex;
static UCanvas *Context;
static TMap<uint32, UTexture2D*> Textures;
static TMap<uint32, FRuntimeCachedFont*> Fonts;
static TMap<FString, uint32> StringTextures;
static TMap<FString, uint32> StringFonts;

static uint32 EmptyTextureSlot;
static uint32 EmptyFontSlot;
//End

void URender2D::FindNextTextureMapSlot()
{
	while (Textures.Find(EmptyTextureSlot)) {
		EmptyTextureSlot++;
	}
}
void URender2D::FindNextFontMapSlot()
{
	while (Fonts.Find(EmptyFontSlot)) {
		EmptyFontSlot++;
	}
}

void URender2D::PushMatrix(FMatrix mat)
{
	Context->Canvas->PushAbsoluteTransform(mat);
}

void URender2D::PopMatrix()
{
	Context->Canvas->PopTransform();
}

void URender2D::SetContext(UCanvas *c)
{
	Context = c;
}

void URender2D::ExitContext()
{
	Context = NULL;
	NoTex = false;
}

//Set draw color for all upcoming draw operations
void URender2D::SetColor(FColor col)
{
	CurColor = col;
}

//Set draw texture for all compatible upcoming draw operations
void URender2D::SetTexture(uint32 id)
{
	CurTexture = id;
	NoTex = false;
}

void URender2D::ResetTexture()
{
	NoTex = true;
}

//Set text font
void URender2D::SetFont(uint32 id)
{
	CurFont = id;
}

//Loads font from path
uint32 URender2D::LoadFont(FString path, uint32 Size, FName ComposeType)
{
	uint32 *ID = StringFonts.Find(path);
	if (ID != NULL) {
		return *ID;
	}

	uint32 id = EmptyFontSlot;
	FString fntp;
	EResolverResult res = UUModAssetsManager::Instance->ResolveAsset(path, EUModAssetType::FONT, fntp);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Unable to load font : %s"), *UUModAssetsManager::Instance->GetErrorMessage(res));
		return id;
	}
	UFont* obj = LoadObjFromPath<UFont>(*fntp);
	obj->AddToRoot(); //Workarround that successfully says "Shout up !" to the peace of crap that Epic Games designed only to break my game : The FUCKING SHITTY GARBAGE COLLECTOR !
	FRuntimeCachedFont *fnt = new FRuntimeCachedFont(obj, Size, ComposeType);
	TSharedRef<FSlateFontCache> FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();
	FCharacterList *lst = &FontCache->GetCharacterList(fnt->SlateFont, 1);
	fnt->CharList = lst;
	Fonts.Add(id, fnt);
	StringFonts.Add(path, id);
	FindNextFontMapSlot();
	return id;
}

//Unloads font
void URender2D::UnloadFont(uint32 id)
{
	EmptyFontSlot = id;
	Fonts.Remove(EmptyFontSlot);
}

//Loads texture from path
uint32 URender2D::LoadTexture(FString path)
{
	uint32 *ID = StringTextures.Find(path);
	if (ID != NULL) {
		return *ID;
	}

	uint32 id = EmptyTextureSlot;
	FString texp;
	EResolverResult res = UUModAssetsManager::Instance->ResolveAsset(path, EUModAssetType::OTHER, texp);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Unable to load texture : %s"), *UUModAssetsManager::Instance->GetErrorMessage(res));
		return id;
	}
	UTexture2D* tex = LoadObjFromPath<UTexture2D>(*texp);
	tex->AddToRoot(); //Workarround that successfully says "Shout up !" to the peace of crap that Epic Games designed only to break my game : The FUCKING SHITTY GARBAGE COLLECTOR !
	Textures.Add(id, tex);
	StringTextures.Add(path, id);
	FindNextTextureMapSlot();
	return id;
}

//Unloads texture
void URender2D::UnloadTexture(uint32 id)
{
	EmptyTextureSlot = id;
	Textures.Remove(EmptyTextureSlot);
}

//Draws a rectangle
void URender2D::DrawRect(float x, float y, float w, float h)
{
	FCanvasTileItem Rectangle(FVector2D(x, y), FVector2D(w, h), FLinearColor(CurColor));
	if (!NoTex && Textures[CurTexture] != NULL) {
		FTexture* tex = Textures[CurTexture]->Resource;
		Rectangle = FCanvasTileItem(FVector2D(x, y), tex, FVector2D(w, h), FLinearColor(CurColor));
	}
	
	Rectangle.BlendMode = SE_BLEND_Translucent;

	Context->DrawItem(Rectangle);	
}

//Draw an outline rectangle
void URender2D::DrawOutlineRect(float x, float y, float w, float h, float stroke)
{
	DrawRect(x, y, x + stroke, h);
	DrawRect((x + w) - stroke, y, stroke, h);
	DrawRect(x, y, w, stroke);
	DrawRect(x, (y + h) - stroke, w, stroke);
}

//Draw polygon with vertex array (NOTE : need special code...)
void URender2D::DrawPoly(TArray<FVertex> vertices)
{

}

//Draw a circle with given rayon (NOTE : need special code...)
void URender2D::DrawCircle(float x, float y, int r)
{

}

//Draw a rounded rectangle (NOTE : need special code...)
void URender2D::DrawRoundedRect(float x, float y, float w, float h, int rayon)
{

}

//Set the scissoring rectangle (NOTE : need special code...)
void URender2D::SetScissorRect(int x, int y, int w, int h)
{
	FCanvasScissorRectItem item = FCanvasScissorRectItem(x, y, w, h);
	Context->DrawItem(item);
}

//Draws a string
void URender2D::DrawText(FString str, float x, float y, uint8 align)
{	
	FRuntimeCachedFont *fnt = Fonts[CurFont];
	if (fnt == NULL) {
		return;
	}

	float TextX = 0;
	float TextY = y;

	float TW, TH;

	switch (align)
	{
	case 0:
		TextX = x;
		break;
	case 1:		
		GetTextSize(str, TW, TH);

		TextX = x - TW / 2;
		break;
	case 2:		
		GetTextSize(str, TW, TH);

		TextX = x - TW;
		break;
	}

	if (!fnt->SlateFont.FontObject->IsValidLowLevel()) {
		UE_LOG(UMod_Game, Error, TEXT("The fucking shit garbage collector of UE4 corrupted UMod memory !"));
		return;
	}

	FCanvasTextItem Text(FVector2D(TextX, TextY), FText::FromString(str), fnt->SlateFont, CurColor);
	Text.Scale = FontScale;
	Context->DrawItem(Text);
}

void URender2D::SetFontScale(float sx, float sy)
{
	FontScale = FVector2D(sx, sy);
}

void URender2D::GetTextSize(FString str, float& w, float& h)
{
	w = 0;
	h = 0;
	//TODO : Remake Context->TextSize function as it does not support runtime cached fonts !
	//NOTE TO EPIC GAMES : Please fix this issue... You allow us to use fonts at runtime with different sizes, but you forget to implement C++ HUD support.
	//Sometimes, there are people that wants to render texts that are intended to change each frame which is not good for UMG.
	//There are also other cases where you want a dynmaic HUD that uses variables from C++ (so UMG is not suitable here)...
	//I know you prefer that we use BP but seriously, I want to say, if you add C++, then you can be sure I will always prefer it as BP !
	FRuntimeCachedFont *fnt = Fonts[CurFont];
	if (fnt == NULL) {
		return;
	}
	TSharedRef<FSlateFontCache> FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();
	FCharacterList &lst = FontCache->GetCharacterList(fnt->SlateFont, 1);
	h = lst.GetMaxHeight() * FontScale.Y;
	for (int i = 0; i < str.Len(); i++) {
		TCHAR c = str[i];		
		FCharacterEntry ce = lst.GetCharacter(c, EFontFallback::FF_NoFallback);
		float advX = (float)ce.XAdvance;
		w += advX * FontScale.X;		
	}
	//UE_LOG(UMod_Game, Log, TEXT("Text width : %f"), w);
}

void URender2D::DrawLine(float x, float y, float x1, float y1, float stroke)
{
	Context->K2_DrawLine(FVector2D(x, y), FVector2D(x1, y1), stroke, FLinearColor(CurColor));
}

bool URender2D::CheckContext()
{
	return Context != NULL;
}
