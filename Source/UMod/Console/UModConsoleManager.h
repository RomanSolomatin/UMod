// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OutputDevice.h"
#include "UModConsoleManager.generated.h"

USTRUCT()
struct FLogLine {

	GENERATED_USTRUCT_BODY()

	FLogLine() {
	}

	FLogLine(FColor col, FString str) {
		LogColor = col;
		LogStr = str;
	}

	UPROPERTY(BlueprintReadOnly)
	FString LogStr;

	UPROPERTY(BlueprintReadOnly)
	FColor LogColor;
};

template<typename T>
struct FUModConsoleVar {
	FUModConsoleVar(T val, FString name, FString desc, bool config, bool sync) {
		Value = val;
		Default = val;
		VarName = name;
		VarDesc = desc;
		ConfigSaved = config;
		Synced = sync;
	}

	T Default;
	T Value;
	FString VarName;
	FString VarDesc;
	bool ConfigSaved;
	bool Synced;
};

struct FUModConsoleCommand {
	FUModConsoleCommand(FString name, FString help, bool ply, bool(*exec)(class AUModCharacter* player, class UUModGameInstance* Game, TArray<FString> params)) {
		CommandName = name;
		CommandHelp = help;
		NeedPlayer = ply;
		ExecFunc = exec;		
	}

	FUModConsoleCommand() {
	}
	
	FString CommandName;
	FString CommandHelp;
	bool NeedPlayer;
	bool(*ExecFunc)(class AUModCharacter* player, class UUModGameInstance* Game, TArray<FString> params);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLogAddedDelegate, FLogLine, LogLine);

class FUModOutputDevice : public FOutputDevice {	
public:
	FUModOutputDevice(class UUModConsoleManager *M);
	~FUModOutputDevice();
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category) override;
private:
	class UUModConsoleManager *Manager;
};

/**
 * 
 */
UCLASS()
class UUModConsoleManager : public UObject
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Logs", Keywords = "get logs"), Category = "UMod_Specific|ConsoleManager")
	TArray<FLogLine>& GetLogs();

	virtual void BeginDestroy();

	UPROPERTY(BlueprintAssignable)
	FLogAddedDelegate OnLogAdded;

	//PUBLIC API START (Again I HATE when UE4 does my work... Console commands/variables is MY WORK as game developer !)
	/*
	TODO : implement those vars :
	SV_LUA_ALLOW_FILE_DL BOOL
	SV_LUA_ALLOW_FILE_ALLTYPE BOOL
	SV_LUA_ALLOW_FILE_ALLPATH BOOL
	SV_LUA_ALLOW_RUN BOOL
	CL_LUA_ALLOW_FILE_DL BOOL
	CL_LUA_ALLOW_FILE_ALLTYPE BOOL
	CL_LUA_ALLOW_FILE_ALLPATH BOOL
	CL_LUA_ALLOW_RUN BOOL
	SV_PHYSICS_UPDRATE INT
	SV_PHYSICS_ALLOWCONSTRAINTDYNAMIC BOOL
	SV_GOD BOOL
	SV_NOCLIP BOOL
	TODO : implement those commands :
	RUNLUA(FString)
	CHMAP(FString)
	CHMODE(FString) #note : don't misunderstand with linux chmod it means change lua gamemode in that case...#
	DISCONNECT()
	EXIT()
	RELOAD() #note : only reloads lua VM, destroy it and recreate it (may be CPU intensive)#
	RECONNECT() #note : only reconnects the same server in case of sync bugs#
	*/

	template<typename T>
	T GetConsoleVar(FString name);

	template<typename T>
	void SetConsoleVar(FString name, T val);

	void DefineConsoleInt(FUModConsoleVar<int> var);
	void DefineConsoleString(FUModConsoleVar<FString> var);
	void DefineConsoleBool(FUModConsoleVar<bool> var);	
	
	void AddLogLine(FString log, FColor col);

	void RunConsoleCommand(FString cmdline, AUModCharacter* player = NULL);
	//PUBLIC API END

	//Those are public for replication using my own var sender/reader
	TArray<FUModConsoleVar<int>> ConsoleIntegers;
	TArray<FUModConsoleVar<bool>> ConsoleBooleans;

	//Commands (Definatly I realy hate UE4 command system !)
	static FUModConsoleCommand** ConsoleCommands;
	static int ConsoleCommandNumber;
	//End
	
	FUModOutputDevice *Out;
	UUModGameInstance *Game;
private:	
	//If we are still allowing vars registration (this way we have a fix number of vars to upload)
	bool RegisteringVars;	

	TArray<FLogLine> Logs;

	//Allowing only 3 different type of console variables
	TArray<FUModConsoleVar<FString>> ConsoleStrings;
};

//Force fucking clang++ to compile the API !!
	template<>
	inline int UUModConsoleManager::GetConsoleVar<int>(FString name)
	{
		for (int i = 0; i < ConsoleIntegers.Num(); i++) {
			if (ConsoleIntegers[i].VarName == name) {
				return ConsoleIntegers[i].Value;
			}
		}
		return 0;
	}
	template<>
	inline bool UUModConsoleManager::GetConsoleVar<bool>(FString name)
	{
		for (int i = 0; i < ConsoleBooleans.Num(); i++) {
			if (ConsoleBooleans[i].VarName == name) {
				return ConsoleBooleans[i].Value;
			}
		}
		return false;
	}
	template<>
	inline FString UUModConsoleManager::GetConsoleVar<FString>(FString name)
	{
		for (int i = 0; i < ConsoleStrings.Num(); i++) {
			if (ConsoleStrings[i].VarName == name) {
				return ConsoleStrings[i].Value;
			}
		}
		return "";
	}

	template<>
	inline void UUModConsoleManager::SetConsoleVar<int>(FString name, int val)
	{
		for (int i = 0; i < ConsoleIntegers.Num(); i++) {
			if (ConsoleIntegers[i].VarName == name) {
				ConsoleIntegers[i].Value = val;
			}
		}
	}
	template<>
	inline void UUModConsoleManager::SetConsoleVar<bool>(FString name, bool val)
	{
		for (int i = 0; i < ConsoleBooleans.Num(); i++) {
			if (ConsoleBooleans[i].VarName == name) {
				ConsoleBooleans[i].Value = val;
			}
		}
	}
	template<>
	inline void UUModConsoleManager::SetConsoleVar<FString>(FString name, FString val)
	{
		for (int i = 0; i < ConsoleStrings.Num(); i++) {
			if (ConsoleStrings[i].VarName == name) {
				ConsoleStrings[i].Value = val;
			}
		}
	}