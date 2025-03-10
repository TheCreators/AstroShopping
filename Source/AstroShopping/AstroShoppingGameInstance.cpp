#include "AstroShoppingGameInstance.h"
#include "SaveSystem/PersistenceSubsystem.h"

void UAstroShoppingGameInstance::Shutdown()
{
	UPersistenceSubsystem* PersistenceSubsystem = GetSubsystem<UPersistenceSubsystem>();
	if (PersistenceSubsystem)
	{
		PersistenceSubsystem->RequestSave(false);
	}

	Super::Shutdown();
}
