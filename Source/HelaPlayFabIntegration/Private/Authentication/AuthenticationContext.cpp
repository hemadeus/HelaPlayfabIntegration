#include "Authentication/AuthenticationContext.h"

void UHelaAuthenticationContext::SetBasicAuthenticationContext(
	FString InEntityToken,
	FString InEntityId,
	FString InEntityType,
	FString InSessionTicket,
	FString InId)
{
	EntityToken = InEntityToken;
	EntityId = InEntityId;
	EntityType = InEntityType;
	SessionTicket = InSessionTicket;
	Id = InId;
}

FAuthenticationData UHelaAuthenticationContext::ToStruct() const
{
	FAuthenticationData Data;
	Data.EntityToken = EntityToken;
	Data.SessionTicket = SessionTicket;
	Data.EntityId = EntityId;
	Data.EntityType = EntityType;
	Data.Id = Id;
	Data.UserName = UserName;
	Data.AvatarUrl = AvatarUrl;
	return Data;
}

UHelaAuthenticationContext* UHelaAuthenticationContext::FromStruct(const FAuthenticationData& Data)
{
	UHelaAuthenticationContext* Context = NewObject<UHelaAuthenticationContext>();
	Context->EntityToken = Data.EntityToken;
	Context->SessionTicket = Data.SessionTicket;
	Context->EntityId = Data.EntityId;
	Context->EntityType = Data.EntityType;
	Context->Id = Data.Id;
	Context->UserName = Data.UserName;
	Context->AvatarUrl = Data.AvatarUrl;
	return Context;
}
