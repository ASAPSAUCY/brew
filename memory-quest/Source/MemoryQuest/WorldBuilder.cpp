#include "WorldBuilder.h"

#include "ColorBlock.h"
#include "Components/LightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "DogActors.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EscapeRoomActors.h"
#include "GameFramework/PlayerStart.h"
#include "KeepsakeItem.h"
#include "MQGameMode.h"
#include "MemoryCreature.h"
#include "TalkNPC.h"
#include "ZoneTrigger.h"

void AWorldBuilder::BuildWorld()
{
	SpawnLighting();
	BuildGrounds();
	BuildHub();
	BuildHallowGrove();
	BuildCourts();
	BuildLittleNewYork();
	BuildCozyPark();
	BuildEscapeRoom();
	SpawnStoryActors();
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

AColorBlock* AWorldBuilder::Block(const FVector& Center, const FVector& Scale, const FLinearColor& Color, EMQShape Shape, const FRotator& Rotation)
{
	const FTransform Transform(Rotation, Center, Scale);
	AColorBlock* Spawned = GetWorld()->SpawnActorDeferred<AColorBlock>(
		AColorBlock::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Spawned != nullptr)
	{
		Spawned->SetBaseColor(Color);
		Spawned->SetShape(Shape);
		Spawned->FinishSpawning(Transform);
	}
	return Spawned;
}

void AWorldBuilder::Zone(const FVector& Center, const FVector& Scale, const FString& Name, const FString& Subtitle)
{
	const FTransform Transform(FRotator::ZeroRotator, Center, Scale);
	AZoneTrigger* Trigger = GetWorld()->SpawnActorDeferred<AZoneTrigger>(
		AZoneTrigger::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Trigger != nullptr)
	{
		Trigger->SetZoneText(Name, Subtitle);
		Trigger->FinishSpawning(Transform);
	}
}

void AWorldBuilder::Tree(const FVector& Base, const FLinearColor& CrownColor, float CrownScale)
{
	Block(FVector(Base.X, Base.Y, Base.Z + 130.f), FVector(0.5f, 0.5f, 2.6f), FLinearColor(0.40f, 0.28f, 0.16f), EMQShape::Cylinder);
	Block(FVector(Base.X, Base.Y, Base.Z + 310.f), FVector(CrownScale, CrownScale, CrownScale), CrownColor, EMQShape::Sphere);
}

void AWorldBuilder::Building(const FVector& Center, float Width, float Depth, float Height, const FLinearColor& WallColor, const FLinearColor& RoofColor, float DoorHalfWidth, bool bWithRoof)
{
	// Enterable buildings are open-air: the fixed top-down camera looks straight
	// down, so a roof would hide the player the moment they walk in.
	const float GroundTop = 10.f;
	const float WallZ = GroundTop + Height * 0.5f;
	const float HalfWidth = Width * 0.5f;
	const float HalfDepth = Depth * 0.5f;
	const FVector WallScaleY(Width / 100.f, 0.9f, Height / 100.f);

	// Back wall (+Y) and side walls.
	Block(FVector(Center.X, Center.Y + HalfDepth, WallZ), WallScaleY, WallColor);
	Block(FVector(Center.X - HalfWidth, Center.Y, WallZ), FVector(0.9f, Depth / 100.f, Height / 100.f), WallColor);
	Block(FVector(Center.X + HalfWidth, Center.Y, WallZ), FVector(0.9f, Depth / 100.f, Height / 100.f), WallColor);

	// Front wall (-Y) in two segments, leaving a door gap in the middle.
	const float FrontY = Center.Y - HalfDepth;
	const float SegmentSpan = HalfWidth - DoorHalfWidth;
	Block(FVector(Center.X - DoorHalfWidth - SegmentSpan * 0.5f, FrontY, WallZ), FVector(SegmentSpan / 100.f, 0.9f, Height / 100.f), WallColor);
	Block(FVector(Center.X + DoorHalfWidth + SegmentSpan * 0.5f, FrontY, WallZ), FVector(SegmentSpan / 100.f, 0.9f, Height / 100.f), WallColor);

	if (bWithRoof)
	{
		Block(FVector(Center.X, Center.Y, GroundTop + Height + 15.f), FVector((Width + 140.f) / 100.f, (Depth + 140.f) / 100.f, 0.3f), RoofColor);
	}
	else
	{
		// A slim colored cornice hints at the roofline without blocking the view.
		Block(FVector(Center.X, Center.Y + HalfDepth, GroundTop + Height + 15.f), FVector((Width + 140.f) / 100.f, 1.2f, 0.3f), RoofColor);
		Block(FVector(Center.X - HalfWidth, Center.Y, GroundTop + Height + 15.f), FVector(1.2f, (Depth + 140.f) / 100.f, 0.3f), RoofColor);
		Block(FVector(Center.X + HalfWidth, Center.Y, GroundTop + Height + 15.f), FVector(1.2f, (Depth + 140.f) / 100.f, 0.3f), RoofColor);
	}
}

// ---------------------------------------------------------------------------
// Grounds and paths
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildGrounds()
{
	const FLinearColor Grass(0.30f, 0.52f, 0.26f);
	const FLinearColor PathColor(0.68f, 0.62f, 0.50f);

	// One giant base lawn, top surface at Z = 0.
	Block(FVector(0.f, 0.f, -25.f), FVector(300.f, 300.f, 0.5f), Grass);

	// Zone plates, top surface at Z = 10.
	Block(FVector(0.f, 0.f, 5.f), FVector(40.f, 40.f, 0.1f), FLinearColor(0.82f, 0.75f, 0.60f));            // hub
	Block(FVector(-6000.f, -6000.f, 5.f), FVector(50.f, 50.f, 0.1f), FLinearColor(0.28f, 0.16f, 0.36f));    // Hallow Grove
	Block(FVector(6000.f, -6000.f, 5.f), FVector(50.f, 50.f, 0.1f), FLinearColor(0.33f, 0.35f, 0.40f));     // The Courts
	Block(FVector(6000.f, 6000.f, 5.f), FVector(50.f, 50.f, 0.1f), FLinearColor(0.42f, 0.42f, 0.46f));      // Little New York
	Block(FVector(-6000.f, 6000.f, 5.f), FVector(50.f, 50.f, 0.1f), FLinearColor(0.36f, 0.62f, 0.30f));     // Cozy Park

	// Fall-colored Montreal Corner patch inside the park, top surface at Z = 24.
	Block(FVector(-7300.f, 7300.f, 12.f), FVector(16.f, 16.f, 0.24f), FLinearColor(0.72f, 0.42f, 0.14f));

	// Diagonal paths from the hub to each zone.
	Block(FVector(-3000.f, -3000.f, 4.f), FVector(65.f, 6.f, 0.08f), PathColor, EMQShape::Cube, FRotator(0.f, -135.f, 0.f));
	Block(FVector(3000.f, -3000.f, 4.f), FVector(65.f, 6.f, 0.08f), PathColor, EMQShape::Cube, FRotator(0.f, -45.f, 0.f));
	Block(FVector(-3000.f, 3000.f, 4.f), FVector(65.f, 6.f, 0.08f), PathColor, EMQShape::Cube, FRotator(0.f, 135.f, 0.f));
	Block(FVector(3000.f, 3000.f, 4.f), FVector(65.f, 6.f, 0.08f), PathColor, EMQShape::Cube, FRotator(0.f, 45.f, 0.f));

	// Zone banners.
	Zone(FVector(0.f, 0.f, 200.f), FVector(44.f, 44.f, 4.f), TEXT("The Apartment"), TEXT("Where he finally asked"));
	Zone(FVector(-6000.f, -6000.f, 200.f), FVector(50.f, 50.f, 4.f), TEXT("Hallow Grove"), TEXT("Where a dark angel met a 'sexy' cop"));
	Zone(FVector(6000.f, -6000.f, 200.f), FVector(50.f, 50.f, 4.f), TEXT("The Courts"), TEXT("Same team, every time"));
	Zone(FVector(6000.f, 6000.f, 200.f), FVector(50.f, 50.f, 4.f), TEXT("Little New York"), TEXT("The birthday weekend that changed everything"));
	Zone(FVector(-6000.f, 6000.f, 200.f), FVector(50.f, 50.f, 4.f), TEXT("Cozy Park"), TEXT("Picnics, putts and one very good boy"));
	Zone(FVector(-7300.f, 7300.f, 220.f), FVector(16.f, 16.f, 4.f), TEXT("Montreal Corner"), TEXT("Fall colors and fries with gravy"));
}

// ---------------------------------------------------------------------------
// The Apartment (hub)
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildHub()
{
	// Kyle's apartment: cream walls, Tiffany-blue trim, red-framed open door.
	Building(FVector(-800.f, -1050.f, 0.f), 1400.f, 600.f, 260.f,
		FLinearColor(0.93f, 0.90f, 0.82f), StoryData::PlayerColor(), 130.f, false);
	Block(FVector(-800.f, -750.f, 300.f), FVector(3.2f, 0.4f, 0.5f), StoryData::PartnerColor()); // red door header
	Block(FVector(-800.f, -1100.f, 14.f), FVector(4.f, 2.f, 0.06f), StoryData::PartnerColor());  // the rug where he asked

	// Fountain.
	Block(FVector(150.f, 100.f, 35.f), FVector(2.2f, 2.2f, 0.5f), FLinearColor(0.55f, 0.55f, 0.60f), EMQShape::Cylinder);
	Block(FVector(150.f, 100.f, 62.f), FVector(1.8f, 1.8f, 0.08f), FLinearColor(0.25f, 0.55f, 0.85f), EMQShape::Cylinder);

	// "The usual" restaurant table, home of the seafood pasta.
	Block(FVector(600.f, -880.f, 50.f), FVector(1.2f, 1.2f, 0.8f), FLinearColor(0.50f, 0.35f, 0.20f), EMQShape::Cylinder);
	Block(FVector(600.f, -880.f, 95.f), FVector(1.6f, 1.6f, 0.08f), FLinearColor(0.92f, 0.90f, 0.85f), EMQShape::Cylinder);
}

// ---------------------------------------------------------------------------
// Hallow Grove
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildHallowGrove()
{
	const FLinearColor HouseWall(0.20f, 0.14f, 0.28f);
	const FLinearColor HouseRoof(0.12f, 0.08f, 0.18f);
	const FLinearColor Pumpkin(0.90f, 0.45f, 0.10f);
	const FLinearColor WindowGlow(0.95f, 0.55f, 0.15f);
	const FLinearColor BareCrown(0.25f, 0.18f, 0.30f);

	// The party house: sealed and decorative (the party is eternal, invite only).
	Building(FVector(-6400.f, -5100.f, 0.f), 1600.f, 1000.f, 350.f, HouseWall, HouseRoof, 0.f, true);
	Block(FVector(-6400.f, -5548.f, 130.f), FVector(1.6f, 0.15f, 2.3f), FLinearColor(0.55f, 0.25f, 0.08f)); // front door
	Block(FVector(-6650.f, -5548.f, 190.f), FVector(1.4f, 0.15f, 1.2f), WindowGlow);
	Block(FVector(-6150.f, -5548.f, 190.f), FVector(1.4f, 0.15f, 1.2f), WindowGlow);

	// Pumpkins.
	Block(FVector(-5600.f, -6300.f, 38.f), FVector(0.75f, 0.75f, 0.75f), Pumpkin, EMQShape::Sphere);
	Block(FVector(-6600.f, -6500.f, 38.f), FVector(0.75f, 0.75f, 0.75f), Pumpkin, EMQShape::Sphere);
	Block(FVector(-5900.f, -6700.f, 38.f), FVector(0.75f, 0.75f, 0.75f), Pumpkin, EMQShape::Sphere);
	Block(FVector(-6800.f, -5600.f, 38.f), FVector(0.75f, 0.75f, 0.75f), Pumpkin, EMQShape::Sphere);
	Block(FVector(-5300.f, -5900.f, 38.f), FVector(0.75f, 0.75f, 0.75f), Pumpkin, EMQShape::Sphere);

	// Bare autumn trees.
	Tree(FVector(-7300.f, -6500.f, 10.f), BareCrown, 1.8f);
	Tree(FVector(-4900.f, -6900.f, 10.f), BareCrown, 1.8f);
	Tree(FVector(-7000.f, -5300.f, 10.f), BareCrown, 1.8f);
	Tree(FVector(-4700.f, -5500.f, 10.f), BareCrown, 1.8f);
}

// ---------------------------------------------------------------------------
// The Courts
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildCourts()
{
	// Basketball court, top surface at Z = 24.
	Block(FVector(5600.f, -6400.f, 12.f), FVector(18.f, 10.f, 0.24f), FLinearColor(0.65f, 0.38f, 0.18f));

	// Hoops.
	Block(FVector(4850.f, -6400.f, 135.f), FVector(0.35f, 0.35f, 2.5f), FLinearColor(0.9f, 0.9f, 0.9f), EMQShape::Cylinder);
	Block(FVector(4820.f, -6400.f, 270.f), FVector(0.15f, 1.6f, 1.1f), FLinearColor(0.95f, 0.95f, 0.95f));
	Block(FVector(6350.f, -6400.f, 135.f), FVector(0.35f, 0.35f, 2.5f), FLinearColor(0.9f, 0.9f, 0.9f), EMQShape::Cylinder);
	Block(FVector(6380.f, -6400.f, 270.f), FVector(0.15f, 1.6f, 1.1f), FLinearColor(0.95f, 0.95f, 0.95f));

	// Open-air billiards corner (a roof would hide it from the top-down camera).
	const FLinearColor Post(0.35f, 0.25f, 0.15f);
	Block(FVector(6400.f, -5900.f, 150.f), FVector(0.3f, 0.3f, 2.8f), Post, EMQShape::Cylinder);
	Block(FVector(7200.f, -5900.f, 150.f), FVector(0.3f, 0.3f, 2.8f), Post, EMQShape::Cylinder);
	Block(FVector(6400.f, -5100.f, 150.f), FVector(0.3f, 0.3f, 2.8f), Post, EMQShape::Cylinder);
	Block(FVector(7200.f, -5100.f, 150.f), FVector(0.3f, 0.3f, 2.8f), Post, EMQShape::Cylinder);
	Block(FVector(6650.f, -5450.f, 80.f), FVector(2.6f, 1.5f, 0.35f), FLinearColor(0.10f, 0.45f, 0.20f));
	Block(FVector(7000.f, -5650.f, 80.f), FVector(2.6f, 1.5f, 0.35f), FLinearColor(0.10f, 0.45f, 0.20f));

	// Flexi's corner: a workout bench.
	Block(FVector(6450.f, -6950.f, 40.f), FVector(2.f, 0.8f, 0.5f), FLinearColor(0.30f, 0.30f, 0.35f));
}

// ---------------------------------------------------------------------------
// Little New York
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildLittleNewYork()
{
	// Skyline: tall towers hug the north and east edges so they never stand
	// between the west-side camera and anything playable.
	Block(FVector(4800.f, 7500.f, 710.f), FVector(7.f, 7.f, 14.f), FLinearColor(0.35f, 0.36f, 0.40f));
	Block(FVector(7300.f, 7300.f, 610.f), FVector(6.f, 6.f, 12.f), FLinearColor(0.28f, 0.30f, 0.34f));
	Block(FVector(7400.f, 5000.f, 510.f), FVector(5.f, 5.f, 10.f), FLinearColor(0.45f, 0.45f, 0.50f));

	// Taxis.
	Block(FVector(5800.f, 5900.f, 40.f), FVector(1.8f, 0.9f, 0.6f), FLinearColor(0.95f, 0.80f, 0.10f));
	Block(FVector(6200.f, 6150.f, 40.f), FVector(1.8f, 0.9f, 0.6f), FLinearColor(0.95f, 0.80f, 0.10f), EMQShape::Cube, FRotator(0.f, 40.f, 0.f));

	// The bottomless brunch corner: a cafe table with two flutes.
	Block(FVector(6650.f, 6500.f, 50.f), FVector(1.f, 1.f, 0.8f), FLinearColor(0.85f, 0.85f, 0.90f), EMQShape::Cylinder);
	Block(FVector(6620.f, 6470.f, 100.f), FVector(0.12f, 0.12f, 0.35f), FLinearColor(0.98f, 0.85f, 0.40f), EMQShape::Cylinder);
	Block(FVector(6680.f, 6530.f, 100.f), FVector(0.12f, 0.12f, 0.35f), FLinearColor(0.98f, 0.85f, 0.40f), EMQShape::Cylinder);
}

// ---------------------------------------------------------------------------
// Cozy Park (+ Montreal Corner)
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildCozyPark()
{
	const FLinearColor Blossom(0.95f, 0.65f, 0.75f);

	// Spring blossom trees.
	Tree(FVector(-6900.f, 6300.f, 10.f), Blossom);
	Tree(FVector(-5300.f, 5300.f, 10.f), Blossom);
	Tree(FVector(-4600.f, 6200.f, 10.f), Blossom);
	Tree(FVector(-6500.f, 4900.f, 10.f), Blossom);

	// The picnic: blanket and bee sting pizza.
	Block(FVector(-5600.f, 5600.f, 12.f), FVector(3.5f, 3.5f, 0.06f), FLinearColor(0.80f, 0.15f, 0.15f));
	Block(FVector(-5600.f, 5600.f, 20.f), FVector(0.9f, 0.9f, 0.1f), FLinearColor(0.95f, 0.75f, 0.35f), EMQShape::Cylinder);

	// Golf green, top surface at Z = 24, with flag.
	Block(FVector(-5000.f, 6800.f, 12.f), FVector(10.f, 10.f, 0.24f), FLinearColor(0.45f, 0.72f, 0.35f));
	Block(FVector(-5000.f, 6800.f, 110.f), FVector(0.12f, 0.12f, 1.8f), FLinearColor(0.95f, 0.95f, 0.95f), EMQShape::Cylinder);
	Block(FVector(-4960.f, 6800.f, 180.f), FVector(0.7f, 0.08f, 0.45f), StoryData::PartnerColor());

	// The outdoor movie theater.
	Block(FVector(-7150.f, 5200.f, 160.f), FVector(0.3f, 9.f, 3.f), FLinearColor(0.95f, 0.95f, 0.95f));
	Block(FVector(-7150.f, 5200.f, 60.f), FVector(0.5f, 9.6f, 1.2f), FLinearColor(0.10f, 0.10f, 0.12f));
	for (float SeatX = -6800.f; SeatX <= -6600.f; SeatX += 200.f)
	{
		for (float SeatY = 5000.f; SeatY <= 5400.f; SeatY += 200.f)
		{
			Block(FVector(SeatX, SeatY, 35.f), FVector(0.6f, 0.6f, 0.5f), FLinearColor(0.40f, 0.15f, 0.15f));
		}
	}

	// Rumor's memorial: the biggest, softest tree in the park and a small stone.
	Tree(FVector(-6300.f, 7200.f, 10.f), FLinearColor(0.98f, 0.90f, 0.95f), 2.8f);
	Block(FVector(-6250.f, 7120.f, 45.f), FVector(0.7f, 0.4f, 0.7f), FLinearColor(0.55f, 0.55f, 0.60f));

	// Montreal Corner: fall maples on the fall-colored patch (top Z = 24).
	Tree(FVector(-7600.f, 7400.f, 24.f), FLinearColor(0.85f, 0.25f, 0.10f));
	Tree(FVector(-7050.f, 7550.f, 24.f), FLinearColor(0.90f, 0.55f, 0.15f));
	Tree(FVector(-7450.f, 6950.f, 24.f), FLinearColor(0.80f, 0.35f, 0.10f));
}

// ---------------------------------------------------------------------------
// The Final Room (escape room)
// ---------------------------------------------------------------------------

void AWorldBuilder::BuildEscapeRoom()
{
	Building(FVector(1100.f, 1275.f, 0.f), 1700.f, 850.f, 300.f,
		FLinearColor(0.16f, 0.15f, 0.22f), FLinearColor(0.10f, 0.10f, 0.14f), 130.f, false);

	// The locked door fills the gap in the front wall.
	GetWorld()->SpawnActor<AEscapeDoor>(AEscapeDoor::StaticClass(), FVector(1100.f, 850.f, 10.f), FRotator::ZeroRotator);

	AMQGameMode* GameMode = GetWorld()->GetAuthGameMode<AMQGameMode>();

	// Three pedestals, pressed in story order: the party, the courts, New York.
	const FVector PedestalSpots[3] = {
		FVector(700.f, 1400.f, 10.f),
		FVector(1100.f, 1500.f, 10.f),
		FVector(1500.f, 1400.f, 10.f)
	};
	for (int32 Index = 0; Index < 3; ++Index)
	{
		const FTransform Transform(FRotator::ZeroRotator, PedestalSpots[Index]);
		APuzzlePedestal* Pedestal = GetWorld()->SpawnActorDeferred<APuzzlePedestal>(
			APuzzlePedestal::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Pedestal != nullptr)
		{
			Pedestal->SetPuzzleIndex(Index);
			Pedestal->FinishSpawning(Transform);
			if (GameMode != nullptr)
			{
				GameMode->RegisterPedestal(Pedestal);
			}
		}
	}

	if (GameMode != nullptr)
	{
		GameMode->SetTrophySpawnPoint(FVector(1100.f, 1200.f, 10.f));
	}
}

// ---------------------------------------------------------------------------
// Story actors: NPCs, creatures, keepsakes, dogs, player start
// ---------------------------------------------------------------------------

void AWorldBuilder::SpawnStoryActors()
{
	UWorld* World = GetWorld();

	const TArray<FMQNpcDef>& Npcs = StoryData::Npcs();
	for (int32 Index = 0; Index < Npcs.Num(); ++Index)
	{
		const FTransform Transform(FRotator(0.f, Npcs[Index].FacingYaw, 0.f), Npcs[Index].SpawnLocation);
		ATalkNPC* Npc = World->SpawnActorDeferred<ATalkNPC>(
			ATalkNPC::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Npc != nullptr)
		{
			Npc->SetNpcIndex(Index);
			Npc->FinishSpawning(Transform);
		}
	}

	const TArray<FMQCreatureDef>& Creatures = StoryData::Creatures();
	for (int32 Index = 0; Index < Creatures.Num(); ++Index)
	{
		if (Creatures[Index].bSpecial)
		{
			World->SpawnActor<ARumorSpirit>(ARumorSpirit::StaticClass(), Creatures[Index].SpawnLocation, FRotator::ZeroRotator);
			continue;
		}

		const FTransform Transform(FRotator::ZeroRotator, Creatures[Index].SpawnLocation);
		AMemoryCreature* Creature = World->SpawnActorDeferred<AMemoryCreature>(
			AMemoryCreature::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Creature != nullptr)
		{
			Creature->SetCreatureIndex(Index);
			Creature->FinishSpawning(Transform);
		}
	}

	const TArray<FMQKeepsakeDef>& Keepsakes = StoryData::Keepsakes();
	for (int32 Index = 0; Index < Keepsakes.Num(); ++Index)
	{
		const FTransform Transform(FRotator::ZeroRotator, Keepsakes[Index].SpawnLocation);
		AKeepsakeItem* Keepsake = World->SpawnActorDeferred<AKeepsakeItem>(
			AKeepsakeItem::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Keepsake != nullptr)
		{
			Keepsake->SetKeepsakeIndex(Index);
			Keepsake->FinishSpawning(Transform);
		}
	}

	// Buddy waits by the apartment; the player appears next to him.
	World->SpawnActor<ACompanionDog>(ACompanionDog::StaticClass(), FVector(-1100.f, -350.f, 12.f), FRotator::ZeroRotator);
	World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FVector(-950.f, -200.f, 130.f), FRotator::ZeroRotator);
}

// ---------------------------------------------------------------------------
// Lighting
// ---------------------------------------------------------------------------

void AWorldBuilder::SpawnLighting()
{
	UWorld* World = GetWorld();

	// Runtime-spawned lights must be Movable - there is no baked lighting.
	ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 3000.f), FRotator(-55.f, 40.f, 0.f));
	if (Sun != nullptr)
	{
		if (ULightComponent* SunLight = Sun->GetLightComponent())
		{
			SunLight->SetMobility(EComponentMobility::Movable);
			SunLight->SetIntensity(5.f);
		}
	}

	ADirectionalLight* Fill = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 3000.f), FRotator(-30.f, 220.f, 0.f));
	if (Fill != nullptr)
	{
		if (ULightComponent* FillLight = Fill->GetLightComponent())
		{
			FillLight->SetMobility(EComponentMobility::Movable);
			FillLight->SetIntensity(1.5f);
			FillLight->SetCastShadows(false);
		}
	}

	ASkyLight* Sky = World->SpawnActor<ASkyLight>(
		ASkyLight::StaticClass(), FVector(0.f, 0.f, 3000.f), FRotator::ZeroRotator);
	if (Sky != nullptr)
	{
		if (USkyLightComponent* SkyComponent = Sky->GetLightComponent())
		{
			SkyComponent->SetMobility(EComponentMobility::Movable);
			SkyComponent->SetIntensity(1.f);
		}
	}

	// Blue sky and horizon instead of a black void.
	World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
}
