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
	if (Audio)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT(Audio->RawData.GetCopy(NULL, NULL)));
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
}


// Called every frame
void UBinauralTestOne::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//test print of variables
	FVector printTest = FVector(GetRange(), GetElevation(), GetAzimuth());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, printTest.ToString());

}

// Called when the HRTF variables need to be calculated
float UBinauralTestOne::GetRange()
{
	Range = (this->GetOwner()->GetActorTransform().GetLocation() - PlayerReference->GetTransform().GetLocation()).Size();
	return Range;
}
float UBinauralTestOne::GetElevation()
{
	Elevation = FMath::Sin((this->GetOwner()->GetTransform().GetLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}
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
