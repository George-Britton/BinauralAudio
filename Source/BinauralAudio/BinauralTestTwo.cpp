// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestTwo.h"

// Sets default values
ABinauralTestTwo::ABinauralTestTwo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);
	AudioPlayerRight = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player Right"));
	AudioPlayerRight->SetupAttachment(this->RootComponent);

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
void ABinauralTestTwo::BeginPlay()
{
	Super::BeginPlay();

	if (Audio) RightAudio = Audio;
	Audio->ChannelOffsets.Empty();
	RightAudio->ChannelOffsets.Empty();
	Audio->ChannelOffsets.Add(2);
	RightAudio->ChannelOffsets.Add(3);
	AudioPlayer->SetSound(Audio);
	AudioPlayerRight->SetSound(RightAudio);

	for(int32 i = 0; i < 360; i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Yellow, FString::SanitizeFloat(DelayArray[i]));
	}
}

// Called every frame
void ABinauralTestTwo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	//test print of variables
	FVector ThisLocation = this->GetActorLocation() - PlayerReference->GetTransform().GetLocation();
	ThisLocation.Normalize();
	FVector Output;
	if(GetAzimuth() > 180)
	{
		Output = 1.1 * FVector(GetElevation(), GetAzimuth(), GetRange()) / (0.9 * GetRange());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Left: " + Output.ToString());
		Output = 0.9 * FVector(GetElevation(), GetAzimuth(), GetRange()) / (1.1 * GetRange());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Right: " + Output.ToString());
	}else
	{
		Output = (0.9 * FVector(GetElevation(), GetAzimuth(), GetRange())) / (1.1 * GetRange());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Left: " + Output.ToString());
		Output = (1.1 * FVector(GetElevation(), GetAzimuth(), GetRange())) / (0.9 * GetRange());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Right: " + Output.ToString());
	}
	*/
}

// Calculates Range between audio source and player
float ABinauralTestTwo::GetRange()
{
	Range = (this->GetActorLocation() - PlayerReference->GetTransform().GetLocation()).Size();
	return Range;
}
// Calculates Elevation of audio source relative to player
float ABinauralTestTwo::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}
// Calculates Azimuth of audio source around player
float ABinauralTestTwo::GetAzimuth()
{
	float AzimuthRange = (this->GetActorLocation() - PlayerReference->GetTransform().GetLocation()).Size2D();
	FVector ForwardPointOfPlayer = PlayerReference->GetActorForwardVector() * AzimuthRange;
	ForwardPointOfPlayer.Normalize();
	FVector ThisLocation = this->GetActorLocation() - PlayerReference->GetTransform().GetLocation();
	ThisLocation.Normalize();
	float Dot = FVector::DotProduct(ThisLocation, ForwardPointOfPlayer);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);
	if (FVector::DotProduct(PlayerReference->GetActorRightVector() * Range, ThisLocation) > 0)
	{
		Azimuth = 360 - Azimuth;
		CloserEar = ECloserEar::LeftEar;
	}
	else CloserEar = ECloserEar::RightEar;

	return Azimuth;
}

// Called when the audio is ready to be played
void ABinauralTestTwo::PlayFirstEar(ECloserEar FirstEar)
{
	EarDelegate.BindUFunction(this, FName("PlaySecondEar"), CloserEar);
	GetWorld()->GetTimerManager().SetTimer(EarTimer, EarDelegate, DelayArray[FMath::RoundFromZero(Azimuth)], false);
	if(FirstEar == ECloserEar::RightEar) AudioPlayer->Play();
	else AudioPlayerRight->Play();
}
void ABinauralTestTwo::PlaySecondEar(ECloserEar SecondEar)
{
	AudioPlayerRight->Play();
	if (SecondEar == ECloserEar::RightEar) AudioPlayer->Play();
	else AudioPlayerRight->Play();
}