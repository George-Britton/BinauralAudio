// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestOne.h"

// Sets default values for this component's properties
UBinauralTestOne::UBinauralTestOne()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	LeftEar = CreateDefaultSubobject<UAudioComponent>(TEXT("Left ear"));
	RightEar = CreateDefaultSubobject<UAudioComponent>(TEXT("Right ear"));
}


// Called when the game starts
void UBinauralTestOne::BeginPlay()
{
	Super::BeginPlay();
	 // Checks if the audio exists

	/*
	if (Audio)
	{
		Audio->ChannelOffsets.Empty();
		RightAudio = Audio;
		Audio->ChannelOffsets.Add(2);
		RightAudio->ChannelOffsets.Add(3);
		LeftEar->SetSound(Audio);
		RightEar->SetSound(RightAudio);
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "No Audio selected at " + this->GetOwner()->GetName() + ", sound will not play");
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}

	if (!PlayerReference)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, this->GetOwner()->GetName() + " has no Player Reference");
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
	*/

	if(!Audio->CookedSpectralTimeData.Num())GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "First null");
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
void UBinauralTestOne::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	/*
	//test print of variables
	FVector printTest = FVector(GetRange(), GetElevation(), GetAzimuth());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, printTest.ToString());
	*/
}

// Calculates Range between audio source and player
float UBinauralTestOne::GetRange()
{
	Range = (this->GetOwner()->GetActorTransform().GetLocation() - PlayerReference->GetTransform().GetLocation()).Size();
	return Range;
}
// Calculates Elevation of audio source relative to player
float UBinauralTestOne::GetElevation()
{
	Elevation = FMath::Sin((this->GetOwner()->GetTransform().GetLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}
// Calculates Azimuth of audio source around player
float UBinauralTestOne::GetAzimuth()
{
	FVector ForwardPointOfPlayer = PlayerReference->GetActorForwardVector();
	ForwardPointOfPlayer.Normalize();
	FVector ThisLocation = this->GetOwner()->GetTransform().GetLocation() - PlayerReference->GetTransform().GetLocation().Normalize();
	float Dot = FVector::DotProduct(ThisLocation, ForwardPointOfPlayer);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);
	if (FVector::DotProduct(PlayerReference->GetActorRightVector() * Range, ThisLocation) > 0) Azimuth = 360 - Azimuth;

	return Azimuth;
}
