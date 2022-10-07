// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DeblasiRESTSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnResponseRecieved, TSharedPtr<FJsonObject> Content, bool bSuccess);


/**
 * 
 */
UCLASS()
class DEBLASIREST_API UDeblasiRESTSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void SendGetRequest(FString URL);
	void OnRecieveGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	FOnResponseRecieved GetRequestResponseDelegate;

	void SendPostRequest(FString URL, TSharedRef<FJsonObject> Content, FString HeaderType = "Content-Type", FString HeaderContent = "application/json");
	void OnRecievePostResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	FOnResponseRecieved PostRequestResponseDelegate;
};
