// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestFour.h"

// Sets default values
ABinauralTestFour::ABinauralTestFour()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);

	DelayArray.Init(0, 361);
	float Up = 0;
	for (int32 AzimuthCount = 0; AzimuthCount < 90; AzimuthCount++)
	{
		DelayArray[AzimuthCount] = Up;
		DelayArray[AzimuthCount + 90] = 0.000625 - Up;
		DelayArray[AzimuthCount + 180] = Up;
		DelayArray[AzimuthCount + 270] = 0.000625 - Up;
		Up += 0.000625 / 90;
	}
}

// Called when the game starts or when spawned
void ABinauralTestFour::BeginPlay()
{
	Super::BeginPlay();

	if (Audio)
	{
		if(FCString::Strcmp(*Audio->GetFullName(), TEXT("WAV")) != 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Must be .wav filetype");
			PrimaryActorTick.bCanEverTick = false;
		}
		Buffer = Audio->RawPCMData;
		Audio->Data
	}	
}

// Called every frame
void ABinauralTestFour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Sets base input variables
	Range = GetRange();
	Elevation = GetElevation();
	Azimuth = GetAzimuth();
}

// Calculates Range between audio source and player
float ABinauralTestFour::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestFour::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestFour::GetAzimuth()
{
	FVector ForwardPoint = PlayerReference->GetActorForwardVector();
	ForwardPoint.Z = 0;
	ForwardPoint.Normalize();
	FVector ThisLoc = this->GetActorLocation() - PlayerReference->GetActorLocation();
	ThisLoc.Z = 0;
	ThisLoc.Normalize();
	float Dot = FVector::DotProduct(ThisLoc, ForwardPoint);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);

	// Gets the closest ear and sets full 360° rotation
	FVector RightPoint = PlayerReference->GetActorRightVector();
	RightPoint.Z = 0;
	RightPoint.Normalize();
	if (FVector::DotProduct(RightPoint, ThisLoc) > 0)
	{
		Azimuth = 360 - Azimuth;
		CloserEar = ECloserEar::RightEar;
	}else CloserEar = ECloserEar::LeftEar;

	return Azimuth;
}

// Generates the output sound
void ABinauralTestFour::CreateSound()
{
	Audio->PreSaveRoot(Audio->GetFullName + TEXT(".sl"));
	FactorySound = SoundSurroundFactory->FactoryCreateBinary(
		this->GetClass(),
		this,
		"OutputSound",
		EObjectFlags::RF_Public,
		this,
		TEXT("WAV"),
		Buffer,
		BufferEnd,
		Warnings
		);

	Audio->PreSaveRoot(Audio->GetFullName + TEXT(".sr"));
	FactorySound = SoundSurroundFactory->FactoryCreateBinary(
		this->GetClass(),
		this,
		"OutputSound",
		EObjectFlags::RF_Public,
		this,
		TEXT("WAV"),
		Buffer,
		BufferEnd,
		Warnings
		);
}

