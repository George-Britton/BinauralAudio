// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioAnalyser.h"

// Sets default values
AAudioAnalyser::AAudioAnalyser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAudioAnalyser::BeginPlay()
{
	Super::BeginPlay();
	if (!Audio->CookedSpectralTimeData.Num())GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "First null");
	else if (!Audio->CookedSpectralTimeData[0].Data.Num()) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Second null");
	else {
		for (int32 i = 0; i < Audio->CookedSpectralTimeData.Num(); i++)
		{
			for (int32 j = 0; j < Audio->CookedSpectralTimeData[i].Data.Num(); j++)
			{
				FString Message = "CookedSpectralTimeData[";
				Message.AppendInt(i);
				Message.Append("]Data[");
				Message.AppendInt(j);
				Message.Append("]: " + FString::SanitizeFloat(Audio->CookedSpectralTimeData[i].Data[j].Magnitude));
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Message);
			}
		}
	}
}

// Called every frame
void AAudioAnalyser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

