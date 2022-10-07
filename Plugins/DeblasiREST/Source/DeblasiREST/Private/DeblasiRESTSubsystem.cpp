// Fill out your copyright notice in the Description page of Project Settings.


#include "DeblasiRESTSubsystem.h"

#include "HttpModule.h"
#include "Json.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UDeblasiRESTSubsystem::SendGetRequest(FString URL)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UDeblasiRESTSubsystem::OnRecieveGetResponse);
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UDeblasiRESTSubsystem::OnRecieveGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	TSharedPtr<FJsonObject> ResponseObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObj);
	
	GetRequestResponseDelegate.Broadcast(ResponseObj, bSuccess);
}

void UDeblasiRESTSubsystem::SendPostRequest(FString URL, TSharedRef<FJsonObject> Content, FString HeaderType,
	FString HeaderContent)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(Content, Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &UDeblasiRESTSubsystem::OnRecievePostResponse);
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader(HeaderType, HeaderContent);
	Request->SetContentAsString(RequestBody);
	Request->ProcessRequest();
}

void UDeblasiRESTSubsystem::OnRecievePostResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	TSharedPtr<FJsonObject> ResponseObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObj);
	
	PostRequestResponseDelegate.Broadcast(ResponseObj, bSuccess);
}
