// Fill out your copyright notice in the Description page of Project Settings.

#include "BinauralTestEleven.h"

// Sets default values
ABinauralTestEleven::ABinauralTestEleven()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the HRTF arrays are initialised
TArray<FHRTFValuesBasedOnElevation> ABinauralTestEleven::InitialiseHRTFs(float MaxDelay, float MaxPitch, float MaxVolume)
{
	TArray<FHRTFValuesBasedOnElevation> HRTFArray;
	HRTFArray.SetNum(360);
	float DelayFiller = 0;
	float PitchFiller = 0;
	float VolumeFiller = 0;
	
	// Fills the spatialisation arrays with sub-second delay values, Hz differences, and dB offsets
	for(int8 AzimuthCounter = 0; AzimuthCounter < 90; AzimuthCounter++)
	{
		HRTFArray[AzimuthCounter].DelayOffset = DelayFiller;
		HRTFArray[AzimuthCounter].PitchOffset = PitchFiller;
		HRTFArray[AzimuthCounter].VolumeOffset = VolumeFiller;
		HRTFArray[AzimuthCounter + 90].DelayOffset = MaxDelay - DelayFiller;
		HRTFArray[AzimuthCounter + 90].PitchOffset = MaxPitch - PitchFiller;
		HRTFArray[AzimuthCounter + 90].VolumeOffset = MaxVolume - VolumeFiller;
		HRTFArray[AzimuthCounter + 180].DelayOffset = DelayFiller;
		HRTFArray[AzimuthCounter + 180].PitchOffset = PitchFiller;
		HRTFArray[AzimuthCounter + 180].VolumeOffset = VolumeFiller;
		HRTFArray[AzimuthCounter + 270].DelayOffset = MaxDelay - DelayFiller;
		HRTFArray[AzimuthCounter + 270].PitchOffset = MaxPitch - PitchFiller;
		HRTFArray[AzimuthCounter + 270].VolumeOffset = MaxVolume - VolumeFiller;
		
		DelayFiller += MaxDelay / 90;
		PitchFiller += MaxPitch / 90;
		VolumeFiller += MaxVolume / 90;
	}

	return HRTFArray;
}

// Called when a value is changed
void ABinauralTestEleven::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// Creates the audio components and attaches them to the root
	for (int32 AudioPlayerSetup = 0; AudioPlayerSetup < BinauralSynthesizer.Num(); AudioPlayerSetup++)
	{
		BinauralSynthesizer[AudioPlayerSetup].AudioPlayer = NewObject<UAudioComponent>(this);
		BinauralSynthesizer[AudioPlayerSetup].AudioPlayer->SetupAttachment(this->RootComponent);
	}

	// Inits the arrays for the HRTF calculations and file paths
	FHRTFMaximums HRTFMaximums;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::SanitizeFloat(HRTFMaximums.MaximumDelayOffsets.At0Degrees));
	HRTF_Minus20Degrees = InitialiseHRTFs(HRTFMaximums.MaximumDelayOffsets.Minus20Degrees, HRTFMaximums.MaximumPitchOffsets.Minus20Degrees, HRTFMaximums.MaximumVolumeOffsets.Minus20Degrees);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::SanitizeFloat(HRTF_Minus20Degrees[89].VolumeOffset));
	//This just printed 0, so the arrays aren't filling
	HRTF_Minus10Degrees = InitialiseHRTFs(HRTFMaximums.MaximumDelayOffsets.Minus10Degrees, HRTFMaximums.MaximumPitchOffsets.Minus10Degrees, HRTFMaximums.MaximumVolumeOffsets.Minus10Degrees);
	HRTF_0Degrees = InitialiseHRTFs(HRTFMaximums.MaximumDelayOffsets.At0Degrees, HRTFMaximums.MaximumPitchOffsets.At0Degrees, HRTFMaximums.MaximumVolumeOffsets.At0Degrees);
	HRTF_Positive10Degrees = InitialiseHRTFs(HRTFMaximums.MaximumDelayOffsets.Positive10Degrees, HRTFMaximums.MaximumPitchOffsets.Positive10Degrees, HRTFMaximums.MaximumVolumeOffsets.Positive10Degrees);
	HRTF_Positive20Degrees = InitialiseHRTFs(HRTFMaximums.MaximumDelayOffsets.Positive20Degrees, HRTFMaximums.MaximumPitchOffsets.Positive20Degrees, HRTFMaximums.MaximumVolumeOffsets.Positive20Degrees);
}

// Called when the game starts or when spawned
void ABinauralTestEleven::BeginPlay()
{
	Super::BeginPlay();

	for (int32 BeginPlayLooper = 0; BeginPlayLooper < BinauralSynthesizer.Num(); BeginPlayLooper++)
	{
		if (BinauralSynthesizer[BeginPlayLooper].Audio)
		{
			// Creates the MixerDevice that holds the sound modifications
			BinauralSynthesizer[BeginPlayLooper].MixerDevice = static_cast<Audio::FMixerDevice*>(GEngine->GetMainAudioDevice());

			// Creates the ActiveSound that will hold the WaveInstances
			BinauralSynthesizer[BeginPlayLooper].ActiveSound->SetSound(BinauralSynthesizer[BeginPlayLooper].Audio);
			BinauralSynthesizer[BeginPlayLooper].ActiveSound->bAllowSpatialization = true;
			
			// Creates new WaveInstances that play the spatialised sound
			UPTRINT LeftHash = 0;
			UPTRINT RightHash = 0;
			BinauralSynthesizer[BeginPlayLooper].LeftSoundPtr = new FWaveInstance(LeftHash, *BinauralSynthesizer[BeginPlayLooper].ActiveSound);
			BinauralSynthesizer[BeginPlayLooper].RightSoundPtr = new FWaveInstance(RightHash, *BinauralSynthesizer[BeginPlayLooper].ActiveSound);

			// Initialises the spatialisation
			if (BinauralSynthesizer[BeginPlayLooper].Audio->GetFullName().Contains("wav")) CreateSound(BeginPlayLooper);
			else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, BinauralSynthesizer[BeginPlayLooper].Audio->GetFullName() + ": must be .wav filetype");
		}
	}
}

// Called every frame
void ABinauralTestEleven::Tick(float DeltaTime)
{
	// Play timer for sound source
	for (int32 IsPlayingLooper = 0; IsPlayingLooper < BinauralSynthesizer.Num(); IsPlayingLooper++)
	{
		if (BinauralSynthesizer[IsPlayingLooper].SoundPlaying && !BinauralSynthesizer[IsPlayingLooper].LeftSoundPtr->bIsFinished && !BinauralSynthesizer[IsPlayingLooper].RightSoundPtr->bIsFinished)
		{
			BinauralSynthesizer[IsPlayingLooper].PickUpTime += DeltaTime;
		}
		else if (BinauralSynthesizer[IsPlayingLooper].LeftSoundPtr->bIsFinished && BinauralSynthesizer[IsPlayingLooper].RightSoundPtr->bIsFinished)
		{
			BinauralSynthesizer[IsPlayingLooper].PickUpTime = -1;
		}
	}
}

// Calculates Range between audio source and player
float ABinauralTestEleven::GetRange(int32 Index)
{
	BinauralSynthesizer[Index].Range = FVector::Dist(BinauralSynthesizer[Index].AudioSource->GetActorLocation(), BinauralSynthesizer[Index].PlayerReference->GetActorLocation());
	return BinauralSynthesizer[Index].Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestEleven::GetElevation(int32 Index)
{
	BinauralSynthesizer[Index].Elevation = FMath::Sin((BinauralSynthesizer[Index].AudioSource->GetActorLocation().Z - BinauralSynthesizer[Index].PlayerReference->GetTransform().GetLocation().Z) / BinauralSynthesizer[Index].Range);
	return BinauralSynthesizer[Index].Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestEleven::GetAzimuth(int32 Index)
{
	FVector ForwardPoint = BinauralSynthesizer[Index].PlayerReference->GetActorForwardVector();
	ForwardPoint.Z = 0;
	ForwardPoint.Normalize();
	FVector ThisLoc = BinauralSynthesizer[Index].AudioSource->GetActorLocation() - BinauralSynthesizer[Index].PlayerReference->GetActorLocation();
	ThisLoc.Z = 0;
	ThisLoc.Normalize();
	float Dot = FVector::DotProduct(ThisLoc, ForwardPoint);
	BinauralSynthesizer[Index].Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);

	// Gets the closest ear and sets full 360° rotation
	FVector RightPoint = BinauralSynthesizer[Index].PlayerReference->GetActorRightVector();
	RightPoint.Z = 0;
	RightPoint.Normalize();
	if (FVector::DotProduct(RightPoint, ThisLoc) > 0)
	{
		BinauralSynthesizer[Index].Azimuth = 360 - BinauralSynthesizer[Index].Azimuth;
		BinauralSynthesizer[Index].CloserEar = ECloserEar::RightEar;
	}
	else BinauralSynthesizer[Index].CloserEar = ECloserEar::LeftEar;

	return BinauralSynthesizer[Index].Azimuth;
}

// Generates the output sound
void ABinauralTestEleven::CreateSound(int32 Index)
{
	// Makes sure the Audio channels and buffer are ready
	BinauralSynthesizer[Index].Audio->NumChannels = 2;
	BinauralSynthesizer[Index].Buffer.Reset();

	// Sets the spatialisation settings for the left and right wave instances
	BinauralSynthesizer[Index].ActiveSound->AddWaveInstance(BinauralSynthesizer[Index].LeftSoundPtr->WaveInstanceHash);
	BinauralSynthesizer[Index].ActiveSound->AddWaveInstance(BinauralSynthesizer[Index].RightSoundPtr->WaveInstanceHash);
	WaveInstanceSetup(Index, BinauralSynthesizer[Index].LeftSoundPtr, ECloserEar::LeftEar);
	WaveInstanceSetup(Index, BinauralSynthesizer[Index].RightSoundPtr, ECloserEar::RightEar);

	// Creates the sound source from the generated buffer
	BinauralSynthesizer[Index].MixerDevice->MaxChannels = 2;
	BinauralSynthesizer[Index].SoundSource = BinauralSynthesizer[Index].MixerDevice->CreateSoundSource();	
}

// Sets up the Wave Instance variables
void ABinauralTestEleven::WaveInstanceSetup(int32 Index, FWaveInstance* WaveToMod, ECloserEar SideEar)
{
	// Sets the default sound and spatialisation variables
	WaveToMod->WaveData = BinauralSynthesizer[Index].Audio;
	WaveToMod->Location = BinauralSynthesizer[Index].PlayerReference->GetActorLocation();
	WaveToMod->ListenerToSoundDistanceForPanning = GetRange(Index);

	// Gets the correct volume, pitch and time differences for each ear's sound
	if (BinauralSynthesizer[Index].CloserEar == SideEar)
	{
		WaveToMod->Pitch = BinauralSynthesizer[Index].BasePitch;
		WaveToMod->SetVolumeMultiplier(BinauralSynthesizer[Index].Volume);
		WaveToMod->StartTime = BinauralSynthesizer[Index].PickUpTime;
	}
	else
	{
		WaveToMod->Pitch = BinauralSynthesizer[Index].BasePitch - FindOffset(Index, EHRTFPoint::Pitch);
		WaveToMod->SetVolumeMultiplier(BinauralSynthesizer[Index].Volume - FindOffset(Index, EHRTFPoint::Volume));
		WaveToMod->StartTime = BinauralSynthesizer[Index].PickUpTime - FindOffset(Index, EHRTFPoint::Delay);
	}

	// Stereo channel format
	ESubmixChannelFormat ChannelFormat = ESubmixChannelFormat::Stereo;

	// Adds wave instance to the buffer
	if (SideEar == ECloserEar::LeftEar)
		BinauralSynthesizer[Index].MixerDevice->Get3DChannelMap(ChannelFormat, WaveToMod, 360 - GetAzimuth(Index), WaveToMod->GetUseSpatialization(), BinauralSynthesizer[Index].Buffer);
	else
		BinauralSynthesizer[Index].MixerDevice->Get3DChannelMap(ChannelFormat, WaveToMod, GetAzimuth(Index), WaveToMod->GetUseSpatialization(), BinauralSynthesizer[Index].Buffer);
	
}

// Finds the correct offsets from the arrays
float ABinauralTestEleven::FindOffset(int32 Index, EHRTFPoint DataPoint)
{
	// Tests the azimuth to see which Elevation array is closest to the required value
	float TestingElevation = GetElevation(Index);

	if (TestingElevation < -15)
	{
		switch (DataPoint)
		{
		case EHRTFPoint::Delay: return HRTF_Minus20Degrees[GetAzimuth(Index)].DelayOffset;
		case EHRTFPoint::Pitch: return HRTF_Minus20Degrees[GetAzimuth(Index)].PitchOffset;
		case EHRTFPoint::Volume: return HRTF_Minus20Degrees[GetAzimuth(Index)].VolumeOffset;
		default: break;
		}
	}else if (TestingElevation >= -15 && TestingElevation < -5)
	{
		switch (DataPoint)
		{
		case EHRTFPoint::Delay: return HRTF_Minus10Degrees[GetAzimuth(Index)].DelayOffset;
		case EHRTFPoint::Pitch: return HRTF_Minus10Degrees[GetAzimuth(Index)].PitchOffset;
		case EHRTFPoint::Volume: return HRTF_Minus10Degrees[GetAzimuth(Index)].VolumeOffset;
		default: break;
		}
	}else if (TestingElevation >= 5 && TestingElevation <= 5)
	{
		switch (DataPoint)
		{
		case EHRTFPoint::Delay: return HRTF_0Degrees[GetAzimuth(Index)].DelayOffset;
		case EHRTFPoint::Pitch: return HRTF_0Degrees[GetAzimuth(Index)].PitchOffset;
		case EHRTFPoint::Volume: return HRTF_0Degrees[GetAzimuth(Index)].VolumeOffset;
		default: break;
		}
	}else if (TestingElevation <= 15 && TestingElevation > 5)
	{
		switch (DataPoint)
		{
		case EHRTFPoint::Delay: return HRTF_Positive10Degrees[GetAzimuth(Index)].DelayOffset;
		case EHRTFPoint::Pitch: return HRTF_Positive10Degrees[GetAzimuth(Index)].PitchOffset;
		case EHRTFPoint::Volume: return HRTF_Positive10Degrees[GetAzimuth(Index)].VolumeOffset;
		default: break;
		}
	} else if (TestingElevation > 15)
	{
		switch (DataPoint)
		{
		case EHRTFPoint::Delay: return HRTF_Positive20Degrees[GetAzimuth(Index)].DelayOffset;
		case EHRTFPoint::Pitch: return HRTF_Positive20Degrees[GetAzimuth(Index)].PitchOffset;
		case EHRTFPoint::Volume: return HRTF_Positive20Degrees[GetAzimuth(Index)].VolumeOffset;
		default: break;
		}
	}

	return 0;
}

// Plays the new binaural sound
void ABinauralTestEleven::PlaySound(int32 Index)
{
	BinauralSynthesizer[Index].SoundSource->Play();
}