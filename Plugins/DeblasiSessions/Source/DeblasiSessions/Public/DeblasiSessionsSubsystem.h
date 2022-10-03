// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DeblasiSessionsSubsystem.generated.h"

// Creating delegates for menu class
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeblasiSessionsOnCreateSessionComplete, bool, bSuccess);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDeblasiSessionsOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeblasiSessionsOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeblasiSessionsOnDestroySessionComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeblasiSessionsOnStartSessionComplete, bool, bSuccess);

/**
 * 
 */
UCLASS()
class DEBLASISESSIONS_API UDeblasiSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	// Custom delegates for Menu class
	FDeblasiSessionsOnCreateSessionComplete DeblasiSessionsOnCreateSessionComplete; 
	FDeblasiSessionsOnFindSessionsComplete DeblasiSessionsOnFindSessionsComplete;
	FDeblasiSessionsOnJoinSessionComplete DeblasiSessionsOnJoinSessionComplete;
	FDeblasiSessionsOnDestroySessionComplete DeblasiSessionsOnDestroySessionComplete;
	FDeblasiSessionsOnStartSessionComplete DeblasiSessionsOnStartSessionComplete;
	
	UDeblasiSessionsSubsystem();

	//
	// To handle session functionality, the menu class will call these
	//

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& Result);
	void DestroySession();
	void StartSession();
	
protected:

	//
	// Internal callbacks that we'll add to the Session Interface delegate list
	// These don't need to be called outside of this class
	//
	
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnFindSessionsComplete(bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bSuccess);
	void OnStartSessionComplete(FName SessionName, bool bSuccess);
	
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	//
	// To add to the online session interface delegate list.
	// We will bind our Subsystem internal callbacks to these
	//
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
