// Fill out your copyright notice in the Description page of Project Settings.

#include "BinauralTestNine.h"

// Sets default values
ABinauralTestNine::ABinauralTestNine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);

	// Inits the arrays for the HRTF calculations and file paths
	DelayArray.Init(0, 360);
	PitchArray.Init(0, 360);
	VolumeArray.Init(0, 360);

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
		VolumeArray[AzimuthCount] = VolumeFiller;
		VolumeArray[AzimuthCount + 90] = 0.1 - VolumeFiller;
		VolumeArray[AzimuthCount + 180] = VolumeFiller;
		VolumeArray[AzimuthCount + 270] = 0.1 - VolumeFiller;
		VolumeFiller += 0.1 / 90;
		PitchArray[AzimuthCount] = PitchFiller;
		PitchArray[AzimuthCount + 90] = 0.22 - PitchFiller;
		PitchArray[AzimuthCount + 180] = PitchFiller;
		PitchArray[AzimuthCount + 270] = 0.22 - PitchFiller;
		PitchFiller += 0.22 / 90;
	}
}

// Called when the game starts or when spawned
void ABinauralTestNine::BeginPlay()
{
	Super::BeginPlay();

	if (Audio)
	{
		FWaveInstance::FWaveInstance(*LeftSound);
		FWaveInstance::FWaveInstance(*RightSound);
		ActiveSound.SetSound(Audio);	
		ActiveSound.bAllowSpatialization = true;
		if (Audio->GetFullName().Contains("wav")) CreateSound();
		else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Audio->GetFullName() + ": must be .wav filetype");
	}
}

// Called every frame
void ABinauralTestNine::Tick(float DeltaTime)
{
	// Play timer for sound source
	if (SoundPlaying && !LeftSound->bIsFinished && !RightSound->bIsFinished) PickUpTime += DeltaTime;
	else if (LeftSound->bIsFinished && RightSound->bIsFinished) PickUpTime = -1;
}

// Calculates Range between audio source and player
float ABinauralTestNine::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestNine::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestNine::GetAzimuth()
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
void ABinauralTestNine::CreateSound()
{
	// Makes sure the Audio channels and buffer are ready
	Audio->NumChannels = 2;
	Buffer.Reset();

	// Sets the spatialisation settings for the left and right wave instances
	ActiveSound.AddWaveInstance(LeftSound->WaveInstanceHash);
	ActiveSound.AddWaveInstance(RightSound->WaveInstanceHash);
	WaveInstanceSetup(LeftSound, ECloserEar::LeftEar);
	WaveInstanceSetup(RightSound, ECloserEar::RightEar);
	
	// Creates the sound source from the generated buffer
	MixerDevice->MaxChannels = 2;
	SoundSource = MixerDevice->CreateSoundSource();
}

// Sets up the Wave Instance variables
void ABinauralTestNine::WaveInstanceSetup(FWaveInstance* WaveToMod, ECloserEar SideEar)
{
	// Sets the default sound and spatialisation variables
	WaveToMod->WaveData = Audio;
	WaveToMod->SetUseSpatialization(true);
	WaveToMod->Location = PlayerReference->GetActorLocation();
	WaveToMod->ListenerToSoundDistanceForPanning = GetRange();

	// Gets the correct volume, pitch and time differences for each ear's sound
	if (CloserEar == SideEar)
	{
		WaveToMod->Pitch = BasePitch;
		WaveToMod->SetVolumeMultiplier(Volume);
		WaveToMod->StartTime = PickUpTime;
	} else
	{
		WaveToMod->Pitch = BasePitch - PitchArray[FMath::FloorToInt(GetAzimuth())];
		WaveToMod->SetVolumeMultiplier(Volume - VolumeArray[FMath::FloorToInt(GetAzimuth())]);
		WaveToMod->StartTime = PickUpTime - DelayArray[FMath::FloorToInt(GetAzimuth())];
	}

	// Adds wave instance to the buffer
	if (SideEar == ECloserEar::LeftEar)
	MixerDevice->Get3DChannelMap(ChannelFormat, WaveToMod, 360 - GetAzimuth(), WaveToMod->GetUseSpatialization(), Buffer);
	else
	MixerDevice->Get3DChannelMap(ChannelFormat, WaveToMod, GetAzimuth(), WaveToMod->GetUseSpatialization(), Buffer);
}

// Plays the new binaural sound
void ABinauralTestNine::PlaySound()
{
	SoundSource->Play();
}