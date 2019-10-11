// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestOne.h"
#include <string>

// Sets default values for this component's properties
UBinauralTestOne::UBinauralTestOne()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	LeftEar = CreateDefaultSubobject<UAudioComponent>(TEXT("Left ear"));
	RightEar = CreateDefaultSubobject<UAudioComponent>(TEXT("Right ear"));
	// ...
}


// Called when the game starts
void UBinauralTestOne::BeginPlay()
{
	Super::BeginPlay();

	LeftEar->SetSound(Audio);
	RightEar->SetSound(Audio);
	// ...
	
}


// Called every frame
void UBinauralTestOne::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Sets the Azimuth, Range and Elevation
	Range = (this->GetOwner()->GetActorTransform().GetLocation() - PlayerReference->GetTransform().GetLocation()).Size();
	Elevation = FMath::Sin((this->GetOwner()->GetTransform().GetLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	float AzimuthRange = (this->GetOwner()->GetActorTransform().GetLocation() - PlayerReference->GetTransform().GetLocation()).Size2D();
	FVector ForwardPointOfPlayer = PlayerReference->GetActorForwardVector() * AzimuthRange;
	ForwardPointOfPlayer.Normalize();
	FVector ThisLocation = this->GetOwner()->GetTransform().GetLocation() - PlayerReference->GetTransform().GetLocation();
	ThisLocation.Normalize();
	float Dot = FVector::DotProduct(ThisLocation, ForwardPointOfPlayer);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI/180);
	if (FVector::DotProduct(PlayerReference->GetActorRightVector() * Range, ThisLocation) > 0) Azimuth = 360 - Azimuth;

	//test print of variables
	FVector printTest = FVector(Range, Elevation, Azimuth);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, printTest.ToString());

	LeftEar->AdjustAttenuation(LeftEarAttentuation);
	RightEar->AdjustAttenuation(RightEarAttentuation);
	// ...
}

