// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestSeven.h"

// Sets default values
ABinauralTestSeven::ABinauralTestSeven()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);
	
	// Inits the arrays for the HRTF calculations and file paths
	DelayArray.Init(0, 360);
	PitchArray.Init(0, 360);
	PitchArray.Init(0, 360);

	// Fills the spatialisation arrays with sub-second delay values, Hz differences, and dB offsets
	float DelayFiller = 0;
	float PitchFiller = 0;
	float VolumeFiller = 0;
	for (int32 AzimuthCount = 0; AzimuthCount < 90; AzimuthCount++)
	{
		DelayArray[AzimuthCount] = DelayFiller;
		DelayArray[AzimuthCount + 90] = 0.000625 - DelayFiller;
		DelayArray[AzimuthCount + 180] = DelayFiller;
		DelayArray[AzimuthCount + 270] = 0.000625 - DelayFiller;
		DelayFiller += 0.000625 / 90;
		PitchArray[AzimuthCount] = PitchFiller;
		PitchArray[AzimuthCount + 90] = 0.22 - PitchFiller;
		PitchArray[AzimuthCount + 180] = PitchFiller;
		PitchArray[AzimuthCount + 270] = 0.22 - PitchFiller;
		PitchFiller += 0.22 / 90;
		VolumeArray[AzimuthCount] = VolumeFiller;
		VolumeArray[AzimuthCount + 90] = 0.1 - VolumeFiller;
		VolumeArray[AzimuthCount + 180] = VolumeFiller;
		VolumeArray[AzimuthCount + 270] = 0.1 - VolumeFiller;
		VolumeFiller += 0.1 / 90;
	}
}

// Called when the game starts or when spawned
void ABinauralTestSeven::BeginPlay()
{
	Super::BeginPlay();

	if (Audio)
	{
		if (FString(*Audio->GetFullName()).Contains(".wav"))GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Must be .wav filetype");
		else CreateSound();
	}
}

// Called every frame
void ABinauralTestSeven::Tick(float DeltaTime)
{
	if (SoundPlaying && !OutputSound->bIsFinished) PickUpTime += DeltaTime;
	else if (OutputSound->bIsFinished) PickUpTime = -1;
}

// Calculates Range between audio source and player
float ABinauralTestSeven::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestSeven::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestSeven::GetAzimuth()
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
	}
	else CloserEar = ECloserEar::LeftEar;

	return Azimuth;
}

// Generates the output sound
void ABinauralTestSeven::CreateSound()
{
	ESubmixChannelFormat ChannelFormat = ESubmixChannelFormat::Stereo;
	Audio->NumChannels = 2;
	OutputSound->SpatializationMethod = ESoundSpatializationAlgorithm::SPATIALIZATION_BinauralSynthesis;
	OutputSound->WaveData = Audio;
	OutputSound->SetUseSpatialization(true);
	OutputSound->Location = PlayerReference->GetActorLocation();
	Buffer.Reset();

	/////////////Left Ear spatialisation//////////////
	if (CloserEar == ECloserEar::LeftEar)
	{
		OutputSound->Pitch = BasePitch + PitchArray[FMath::FloorToInt(GetAzimuth())];
		OutputSound->SetVolumeMultiplier(Volume + VolumeArray[FMath::FloorToInt(GetAzimuth())]);
		OutputSound->StartTime = PickUpTime + DelayArray[FMath::FloorToInt(GetAzimuth())];
	} else
	{
		OutputSound->Pitch = BasePitch;
		OutputSound->SetVolumeMultiplier(Volume);
		OutputSound->StartTime = PickUpTime;
	}
	MixerDevice.Get3DChannelMap(ChannelFormat, OutputSound, 360 - GetAzimuth(), OutputSound->GetUseSpatialization(), Buffer);

	
	/////////////Right Ear spatialisation/////////////
	if (CloserEar == ECloserEar::RightEar)
	{
		OutputSound->Pitch = BasePitch + PitchArray[FMath::FloorToInt(GetAzimuth())];
		OutputSound->SetVolumeMultiplier(Volume + VolumeArray[FMath::FloorToInt(GetAzimuth())]);
		OutputSound->StartTime = PickUpTime + DelayArray[FMath::FloorToInt(GetAzimuth())];
	}
	else
	{
		OutputSound->Pitch = BasePitch;
		OutputSound->SetVolumeMultiplier(Volume);
		OutputSound->StartTime = PickUpTime;
	}
	MixerDevice.Get3DChannelMap(ChannelFormat, OutputSound, GetAzimuth(), OutputSound->GetUseSpatialization(), Buffer);

	MixerDevice.MaxChannels = 2;
	SoundSource = MixerDevice.CreateSoundSource();
}

// Plays the new binaural sound
void ABinauralTestSeven::PlaySound()
{
	SoundSource->Play();
}