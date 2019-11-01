// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestFive.h"

// Sets default values
ABinauralTestFive::ABinauralTestFive()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);

	// Inits the arrays for the HRTF calculations and file paths
	DelayArray.Init(0, 360);
	PathNames.Init("", 2);
	NewPathNames.Init("", 2);

	// Fills the delay array with sub-second delay values
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
void ABinauralTestFive::BeginPlay()
{
	Super::BeginPlay();

	if (Audio)
	{
		if (FString(*Audio->GetFullName()).Contains(".wav"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Must be .wav filetype");
			PrimaryActorTick.bCanEverTick = false;
		}
		else
		{
			CreateSound();
			//Aaron: create the fecking reimport surround factory!
			//ReimportSoundSurroundFactory = NewObject<UReimportSoundSurroundFactory>();
			
			/////////////////////////////////////////////////////
			///				TO DO: HRTF CALCS				  ///
			/////////////////////////////////////////////////////
			if (ReimportSoundSurroundFactory->CanReimport(FactorySound, PathNames))
			{
				ReimportSoundSurroundFactory->SetReimportPaths(FactorySound, NewPathNames);
				ReimportSoundSurroundFactory->Reimport(FactorySound);
				OutputSound = Cast<USoundWave>(FactorySound);
			}
		}
	}
}

// Called every frame
void ABinauralTestFive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Sets base input variables
	Range = GetRange();
	Elevation = GetElevation();
	Azimuth = GetAzimuth();
}

// Calculates Range between audio source and player
float ABinauralTestFive::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestFive::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestFive::GetAzimuth()
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
void ABinauralTestFive::CreateSound()
{
	// Creates the left speaker's sound
	LeftFileName = LexToString(FPaths::GameContentDir());
	LeftFileName.Append("Binaural_sl");
	PathNames[0] = LeftFileName;

	//make the fecking factory!
	SoundSurroundFactory = NewObject<USoundSurroundFactory>();
	
	FString FinalPackageName = TEXT("/Game");
	UPackage* Package = CreatePackage(NULL, *FinalPackageName);
	USoundWave* left_sound = Cast<USoundWave>(SoundSurroundFactory->FactoryCreateNew(USoundWave::StaticClass(), Package->GetOutermost(), TEXT("Binaural_sl"), RF_Standalone | RF_Public, NULL, GWarn));
	if(left_sound)
	{
		
	}

	USoundWave* right_sound = Cast<USoundWave>(SoundSurroundFactory->FactoryCreateNew(USoundWave::StaticClass(), Package->GetOutermost(), TEXT("Binaural_sr"), RF_Standalone | RF_Public, NULL, GWarn));
	if(right_sound)
	{
		
	}
	
	/*LeftOutput = Cast<USoundWave>(SoundSurroundFactory->FactoryCreateBinary(
		OutputSound->GetClass(),
		Audio,
		FName(*LeftFileName),
		EObjectFlags::RF_Public,
		this,
		TEXT("WAV"),
		Buffer,
		BufferEnd,
		Warnings
		));*/
	
	// Creates the right speaker's sound
	/*RightFileName = LexToString(FPaths::GameContentDir());
	RightFileName.Append("Binaural_sr");
	PathNames[1] = RightFileName;
	RightOutput = Cast<USoundWave>(SoundSurroundFactory->FactoryCreateBinary(
		this->GetClass(),
		this,
		FName(*RightFileName),
		EObjectFlags::RF_Public,
		this,
		TEXT("WAV"),
		Buffer,
		BufferEnd,
		Warnings
		));*/
}

// Plays the new binaural sound
void ABinauralTestFive::PlaySound()
{
	
}