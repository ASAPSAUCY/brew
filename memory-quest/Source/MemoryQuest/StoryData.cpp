#include "StoryData.h"

// ============================================================================
//  All of the game's personal content. Edit freely - dates, jokes, memories.
//  Locations are world coordinates: the Apartment hub is at (0,0); Hallow
//  Grove (-6000,-6000); The Courts (6000,-6000); Cozy Park (-6000,6000) with
//  the Montreal Corner patch at (-7300,7300); Little New York (6000,6000).
// ============================================================================

FString StoryData::PlayerName() { return TEXT("Kyle"); }
FString StoryData::PartnerName() { return TEXT("Claudia"); }
FLinearColor StoryData::PlayerColor() { return FLinearColor(0.04f, 0.73f, 0.71f); }  // Tiffany blue
FLinearColor StoryData::PartnerColor() { return FLinearColor(0.85f, 0.08f, 0.12f); } // Red

FString StoryData::GameTitle() { return TEXT("MEMORY QUEST"); }
FString StoryData::GameSubtitle() { return TEXT("Kyle & Claudia - four years in the making"); }

int32 StoryData::CreaturesNeededForEscapeRoom() { return 8; }
int32 StoryData::KeepsakesNeededForEscapeRoom() { return 6; }

const TArray<FMQCreatureDef>& StoryData::Creatures()
{
	static const TArray<FMQCreatureDef> Defs = []()
	{
		TArray<FMQCreatureDef> A;

		{
			FMQCreatureDef D;
			D.Name = TEXT("Angelbat");
			D.Species = TEXT("Dark Angel of the Party");
			D.Zone = TEXT("Hallow Grove");
			D.MemoryTitle = TEXT("The Night It Started");
			D.MemoryText = TEXT("Halloween, six years ago. A dark angel walked into the party and Kyle forgot his own (fake) badge number. Two more years of 'just friends' followed. Slowest arrest in history.");
			D.AttackA = TEXT("Wing Flick");
			D.AttackB = TEXT("Smolder");
			D.Shape = EMQShape::Cube;
			D.Size = 1.f;
			D.BodyColor = FLinearColor(0.10f, 0.08f, 0.14f);
			D.TopColor = FLinearColor(0.80f, 0.10f, 0.15f);
			D.MaxHP = 80;
			D.SpawnLocation = FVector(-6350.f, -5500.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Copcake");
			D.Species = TEXT("Off-Duty Cupcake");
			D.Zone = TEXT("Hallow Grove");
			D.MemoryTitle = TEXT("Exhibit B");
			D.MemoryText = TEXT("One 'sexy police officer' costume, as worn by Kyle. The handcuffs were plastic. The crush was extremely real.");
			D.AttackA = TEXT("Ticket Tease");
			D.AttackB = TEXT("Donut Roll");
			D.Shape = EMQShape::Cylinder;
			D.Size = 0.9f;
			D.BodyColor = FLinearColor(0.10f, 0.15f, 0.40f);
			D.TopColor = FLinearColor(0.95f, 0.78f, 0.20f);
			D.MaxHP = 70;
			D.SpawnLocation = FVector(-5350.f, -6650.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Swish");
			D.Species = TEXT("Court Spirit");
			D.Zone = TEXT("The Courts");
			D.MemoryTitle = TEXT("Same Team, Every Time");
			D.MemoryText = TEXT("Mid-COVID pickup games with the crew - the weekly excuse to see each other. Somehow, mysteriously, always on the same team.");
			D.AttackA = TEXT("Crossover");
			D.AttackB = TEXT("Buzzer Beater");
			D.Shape = EMQShape::Sphere;
			D.Size = 1.f;
			D.BodyColor = FLinearColor(0.85f, 0.40f, 0.10f);
			D.TopColor = FLinearColor(0.12f, 0.12f, 0.12f);
			D.MaxHP = 70;
			D.SpawnLocation = FVector(5500.f, -6350.f, 24.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Chalky");
			D.Species = TEXT("8-Ball Oracle");
			D.Zone = TEXT("The Courts");
			D.MemoryTitle = TEXT("Rack 'Em");
			D.MemoryText = TEXT("Post-basketball billiards: 'just one game' that became four years of trash talk. Chalky has witnessed every miracle shot and every excuse.");
			D.AttackA = TEXT("Bank Shot");
			D.AttackB = TEXT("Scratch");
			D.Shape = EMQShape::Sphere;
			D.Size = 0.9f;
			D.BodyColor = FLinearColor(0.06f, 0.06f, 0.08f);
			D.TopColor = FLinearColor(0.95f, 0.95f, 0.95f);
			D.MaxHP = 75;
			D.SpawnLocation = FVector(6800.f, -5300.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Flexi");
			D.Species = TEXT("Gains Goblin");
			D.Zone = TEXT("The Courts");
			D.MemoryTitle = TEXT("Claudia's Era");
			D.MemoryText = TEXT("Claudia's current arc: fitness. Flexi respects the grind, spots her lifts, and has formally requested that Kyle stop calling stretching 'cardio'.");
			D.AttackA = TEXT("Rep Count");
			D.AttackB = TEXT("Protein Slap");
			D.Shape = EMQShape::Cube;
			D.Size = 1.f;
			D.BodyColor = FLinearColor(0.75f, 0.10f, 0.12f);
			D.TopColor = FLinearColor(0.75f, 0.78f, 0.82f);
			D.MaxHP = 85;
			D.SpawnLocation = FVector(6500.f, -6800.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Pidge");
			D.Species = TEXT("Big City Bird");
			D.Zone = TEXT("Little New York");
			D.MemoryTitle = TEXT("The Birthday Weekend");
			D.MemoryText = TEXT("New York, her birthday. A city of eight million people, and they spent the whole weekend only noticing each other. (This pigeon noticed everything.)");
			D.AttackA = TEXT("Coo");
			D.AttackB = TEXT("Crumb Heist");
			D.Shape = EMQShape::Sphere;
			D.Size = 0.7f;
			D.BodyColor = FLinearColor(0.45f, 0.47f, 0.52f);
			D.TopColor = FLinearColor(0.30f, 0.55f, 0.45f);
			D.MaxHP = 60;
			D.SpawnLocation = FVector(5400.f, 5600.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Mimossy");
			D.Species = TEXT("Brunch Spirit");
			D.Zone = TEXT("Little New York");
			D.MemoryTitle = TEXT("Bottomless, 2023");
			D.MemoryText = TEXT("Bottomless mimosas: Claudia's greatest triumph and swiftest downfall. One escape room did not survive the aftermath. The only room left unfinished... until today?");
			D.AttackA = TEXT("Bubbly Burst");
			D.AttackB = TEXT("Second Round");
			D.Shape = EMQShape::Cylinder;
			D.Size = 1.f;
			D.BodyColor = FLinearColor(0.95f, 0.60f, 0.15f);
			D.TopColor = FLinearColor(0.98f, 0.85f, 0.40f);
			D.MaxHP = 90;
			D.SpawnLocation = FVector(6600.f, 6600.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Seafoodle");
			D.Species = TEXT("The Usual");
			D.Zone = TEXT("The Apartment");
			D.MemoryTitle = TEXT("Kyle's Order");
			D.MemoryText = TEXT("Seafood pasta. Every restaurant, every city, every time, forever. The waiters have stopped asking. The pasta has achieved legend status.");
			D.AttackA = TEXT("Noodle Whip");
			D.AttackB = TEXT("Garlic Cloud");
			D.Shape = EMQShape::Cylinder;
			D.Size = 0.8f;
			D.BodyColor = FLinearColor(0.92f, 0.86f, 0.70f);
			D.TopColor = FLinearColor(0.95f, 0.45f, 0.35f);
			D.MaxHP = 65;
			D.SpawnLocation = FVector(600.f, -700.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Slice");
			D.Species = TEXT("Picnic Guardian");
			D.Zone = TEXT("Cozy Park");
			D.MemoryTitle = TEXT("Park Picnics");
			D.MemoryText = TEXT("One blanket, one pizza box, zero regrets. The official venue of their best ordinary afternoons.");
			D.AttackA = TEXT("Cheese Pull");
			D.AttackB = TEXT("Crust Punch");
			D.Shape = EMQShape::Cube;
			D.Size = 0.8f;
			D.BodyColor = FLinearColor(0.95f, 0.80f, 0.30f);
			D.TopColor = FLinearColor(0.80f, 0.15f, 0.10f);
			D.MaxHP = 60;
			D.SpawnLocation = FVector(-5600.f, 5600.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("BeeSting");
			D.Species = TEXT("Hot Honey Bee");
			D.Zone = TEXT("Cozy Park");
			D.MemoryTitle = TEXT("The Official Pizza");
			D.MemoryText = TEXT("Bee sting pizza: hot honey, big feelings. The house pizza of this entire relationship. This bee considers itself a co-founder.");
			D.AttackA = TEXT("Buzz");
			D.AttackB = TEXT("Honey Drizzle");
			D.Shape = EMQShape::Sphere;
			D.Size = 0.6f;
			D.BodyColor = FLinearColor(0.95f, 0.80f, 0.10f);
			D.TopColor = FLinearColor(0.10f, 0.10f, 0.10f);
			D.MaxHP = 55;
			D.SpawnLocation = FVector(-5450.f, 5780.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Bogey");
			D.Species = TEXT("Golf Gremlin");
			D.Zone = TEXT("Cozy Park");
			D.MemoryTitle = TEXT("Kyle's True Rival");
			D.MemoryText = TEXT("Golf gave them long afternoons together - and gave Kyle a lifelong nemesis that lives rent-free on every scorecard. Claudia keeps the receipts.");
			D.AttackA = TEXT("Shank");
			D.AttackB = TEXT("Mulligan");
			D.Shape = EMQShape::Sphere;
			D.Size = 0.8f;
			D.BodyColor = FLinearColor(0.25f, 0.60f, 0.25f);
			D.TopColor = FLinearColor(0.95f, 0.95f, 0.95f);
			D.MaxHP = 85;
			D.SpawnLocation = FVector(-5000.f, 6700.f, 24.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Reely");
			D.Species = TEXT("Matinee Spirit");
			D.Zone = TEXT("Cozy Park");
			D.MemoryTitle = TEXT("Movie #400-and-Something");
			D.MemoryText = TEXT("Claudia picks the movie. Kyle 'just rests his eyes' for a minute. Both of them swear they watched it. Neither of them did.");
			D.AttackA = TEXT("Plot Twist");
			D.AttackB = TEXT("Spoiler");
			D.Shape = EMQShape::Cylinder;
			D.Size = 0.9f;
			D.BodyColor = FLinearColor(0.15f, 0.15f, 0.18f);
			D.TopColor = FLinearColor(0.80f, 0.15f, 0.15f);
			D.MaxHP = 70;
			D.SpawnLocation = FVector(-6800.f, 5350.f, 12.f);
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Poutine");
			D.Species = TEXT("Gravy Elemental");
			D.Zone = TEXT("Montreal Corner");
			D.MemoryTitle = TEXT("The Montreal Getaway");
			D.MemoryText = TEXT("Fall colors, old streets, fries drowned in gravy - and proof they travel well together. Claudia's French half called it 'acceptable'. That's high praise.");
			D.AttackA = TEXT("Gravy Wave");
			D.AttackB = TEXT("Squeak");
			D.Shape = EMQShape::Cube;
			D.Size = 0.9f;
			D.BodyColor = FLinearColor(0.55f, 0.35f, 0.15f);
			D.TopColor = FLinearColor(0.90f, 0.75f, 0.30f);
			D.MaxHP = 75;
			D.SpawnLocation = FVector(-7300.f, 7300.f, 26.f);
			D.WanderRadius = 300.f;
			A.Add(D);
		}
		{
			FMQCreatureDef D;
			D.Name = TEXT("Rumor");
			D.Species = TEXT("Forever Good Boy");
			D.Zone = TEXT("Cozy Park");
			D.MemoryTitle = TEXT("Rumor");
			D.MemoryText = TEXT("Claudia's best boy. He ran ahead last year, but his pawprints are on everything here. Buddy keeps his spot warm. Forever part of this story.");
			D.Shape = EMQShape::Cube;
			D.Size = 1.f;
			D.BodyColor = FLinearColor(0.85f, 0.95f, 1.f);
			D.TopColor = FLinearColor(0.85f, 0.95f, 1.f);
			D.MaxHP = 1;
			D.SpawnLocation = FVector(-6150.f, 7250.f, 12.f);
			D.bSpecial = true;
			A.Add(D);
		}

		return A;
	}();
	return Defs;
}

const TArray<FMQKeepsakeDef>& StoryData::Keepsakes()
{
	static const TArray<FMQKeepsakeDef> Defs = []()
	{
		TArray<FMQKeepsakeDef> A;

		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Party Mask");
			K.MemoryTitle = TEXT("The Costume");
			K.MemoryText = TEXT("A little dark-angel mask. Some costumes you never really take off.");
			K.Color = FLinearColor(0.45f, 0.05f, 0.10f);
			K.SpawnLocation = FVector(-6800.f, -6100.f, 12.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Billiard Chalk");
			K.MemoryTitle = TEXT("House Rules");
			K.MemoryText = TEXT("Blue chalk from the hall. For when the trash talk requires precision.");
			K.Color = FLinearColor(0.15f, 0.30f, 0.80f);
			K.SpawnLocation = FVector(7000.f, -5600.f, 12.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Subway Token");
			K.MemoryTitle = TEXT("One Ride");
			K.MemoryText = TEXT("A token from THE weekend. Good for one ride and everything that came after.");
			K.Color = FLinearColor(0.72f, 0.55f, 0.20f);
			K.SpawnLocation = FVector(5300.f, 6300.f, 12.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Mimosa Flute");
			K.MemoryTitle = TEXT("It Has Seen Things");
			K.MemoryText = TEXT("An empty flute from the 2023 brunch. Handle with respect. And possibly gloves.");
			K.Color = FLinearColor(0.98f, 0.82f, 0.45f);
			K.SpawnLocation = FVector(6750.f, 6450.f, 12.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Movie Stub");
			K.MemoryTitle = TEXT("Feature Presentation");
			K.MemoryText = TEXT("Ticket #400-something. Feature presentation: her head on his shoulder.");
			K.Color = FLinearColor(0.90f, 0.85f, 0.70f);
			K.SpawnLocation = FVector(-7000.f, 5100.f, 12.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Golf Scorecard");
			K.MemoryTitle = TEXT("The Eraser Mark");
			K.MemoryText = TEXT("One scorecard, one suspicious eraser mark. Kyle denies everything. The gremlin knows.");
			K.Color = FLinearColor(0.92f, 0.92f, 0.92f);
			K.SpawnLocation = FVector(-4800.f, 6900.f, 24.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Maple Leaf");
			K.MemoryTitle = TEXT("Red, Her Color");
			K.MemoryText = TEXT("A perfect red maple leaf from Montreal. Red - her color, her season.");
			K.Color = FLinearColor(0.85f, 0.15f, 0.08f);
			K.SpawnLocation = FVector(-7500.f, 7100.f, 26.f);
			A.Add(K);
		}
		{
			FMQKeepsakeDef K;
			K.Name = TEXT("Pizza Box");
			K.MemoryTitle = TEXT("Extra Honey");
			K.MemoryText = TEXT("A flattened bee sting pizza box, saved like a trophy. Extra hot honey, obviously.");
			K.Color = FLinearColor(0.80f, 0.65f, 0.40f);
			K.SpawnLocation = FVector(-5700.f, 5450.f, 12.f);
			A.Add(K);
		}

		return A;
	}();
	return Defs;
}

const TArray<FMQNpcDef>& StoryData::Npcs()
{
	static const TArray<FMQNpcDef> Defs = []()
	{
		TArray<FMQNpcDef> A;

		{
			FMQNpcDef N;
			N.Name = TEXT("Claudia");
			N.Color = PartnerColor();
			N.SpawnLocation = FVector(-750.f, -550.f, 12.f);
			N.FacingYaw = 90.f;
			N.Pages = {
				TEXT("Four years, Kyle. From a dark angel and - I'm quoting YOU here - a 'sexy police officer' to... you built me a whole world? Okay. Okay, I'm a little impressed."),
				TEXT("Our memories got loose, by the way. They're wandering around out there - the bee sting pizza, the pigeons, that little golf gremlin you SWEAR isn't based on you."),
				TEXT("Catch them, grab our keepsakes, then meet me at The Final Room. We are FINALLY finishing that escape room. I have unfinished business with that place."),
				TEXT("And yes - he has exactly ONE joke about me: half Black, half French, palest girl in every room. It stopped being funny in year two. ...It's still a little funny."),
				TEXT("(She grins.) I'd do all four years again. Even the mimosa thing. ESPECIALLY the mimosa thing. Now go - Buddy's waiting.")
			};
			A.Add(N);
		}
		{
			FMQNpcDef N;
			N.Name = TEXT("Boo-Regard");
			N.Color = FLinearColor(0.85f, 0.85f, 0.95f);
			N.SpawnLocation = FVector(-5850.f, -6250.f, 12.f);
			N.Pages = {
				TEXT("BOO! ...Nothing? Tough crowd. You know, I haunted a Halloween party right here, six years back. A dark angel walked in, and some cop in a VERY unregulation uniform forgot his own fake badge number."),
				TEXT("And then they stayed 'just friends' for TWO YEARS. Two! Even ghosts move faster than that, and we are famously dead.")
			};
			A.Add(N);
		}
		{
			FMQNpcDef N;
			N.Name = TEXT("Hoops");
			N.Color = FLinearColor(0.90f, 0.50f, 0.15f);
			N.SpawnLocation = FVector(5800.f, -5900.f, 12.f);
			N.Pages = {
				TEXT("Mid-COVID, this court was the whole social calendar. Same crew every week - and those two 'just friends' somehow always ended up on the same team."),
				TEXT("After hoops it was billiards, every time. 'Just one game.' Sure. Chalky's still holding the 8-ball from their longest argument."),
				TEXT("Claudia's all about the gym these days. Flexi over there has watched Kyle's warm-up routine and has QUESTIONS.")
			};
			A.Add(N);
		}
		{
			FMQNpcDef N;
			N.Name = TEXT("Mr. Marlowe");
			N.Color = FLinearColor(0.12f, 0.16f, 0.35f);
			N.SpawnLocation = FVector(5700.f, 6300.f, 12.f);
			N.Pages = {
				TEXT("Welcome to Little New York. Big birthday weekend, a few years back - two 'friends' checked in... and let's just say they did not check out as friends."),
				TEXT("He asked her properly back at his apartment, right after the trip. Good man. Terrible poker face, but a good man."),
				TEXT("The brunch spirit by the mimosa table? Ask her about 2023. Actually - don't. We're still finding glitter.")
			};
			A.Add(N);
		}
		{
			FMQNpcDef N;
			N.Name = TEXT("Ranger Fern");
			N.Color = FLinearColor(0.20f, 0.45f, 0.20f);
			N.SpawnLocation = FVector(-5900.f, 6100.f, 12.f);
			N.Pages = {
				TEXT("Spring blossoms on this side, fall maples over on the Montreal Corner. Somebody's favorite seasons - both of them. The fries with gravy are for her French half. Oui oui."),
				TEXT("Best picnic in the park: that blanket right there. Bee sting pizza and a seafood pasta, every single time. The bees have become... invested."),
				TEXT("The quiet tree up north is Rumor's spot. Good boy earned a good view. Buddy visits when he thinks no one's looking.")
			};
			A.Add(N);
		}
		{
			FMQNpcDef N;
			N.Name = TEXT("Ms. Locke");
			N.Color = FLinearColor(0.35f, 0.15f, 0.40f);
			N.SpawnLocation = FVector(1450.f, 600.f, 12.f);
			N.FacingYaw = 90.f;
			N.Pages = {
				TEXT("Welcome back to The Final Room. Ah. It's YOU two. I remember the... incident of 2023. We remember it every time it rains, actually."),
				TEXT("New house rule, instituted in your honor: no bottomless brunch before booking. The carpet did not make it."),
				TEXT("The door opens only for those who carry their story with them: 8 memories caught, 6 keepsakes found. Inside, press the pedestals in the order it all happened. Then finish what you started.")
			};
			A.Add(N);
		}

		return A;
	}();
	return Defs;
}

FString StoryData::BuddyName() { return TEXT("Buddy"); }

const TArray<FString>& StoryData::BuddyPages()
{
	static const TArray<FString> Pages = {
		TEXT("Woof! Woof woof! (Translation: I am the goodest boy currently on this quest, and we both know it.)"),
		TEXT("(Buddy would like you to know that Rumor taught him his best trick: loving Claudia.)")
	};
	return Pages;
}

const TArray<FString>& StoryData::PuzzleClues()
{
	static const TArray<FString> Clues = {
		TEXT("A tiny pumpkin rests on this pedestal."),
		TEXT("A tiny basketball rests on this pedestal."),
		TEXT("A tiny yellow taxi rests on this pedestal.")
	};
	return Clues;
}

FString StoryData::PuzzleWrongOrderText()
{
	return TEXT("The room buzzes disapprovingly. Somewhere, a mimosa laughs. The pedestals reset.");
}

FString StoryData::PuzzleSolvedText()
{
	return TEXT("Click. Clunk. The Final Room concedes. FINALLY.");
}

const TArray<FString>& StoryData::FinaleLines()
{
	static const TArray<FString> Lines = {
		TEXT("FINISHED. FINALLY."),
		TEXT("Four years: every slice of bee sting pizza, every 'one more game', every movie Kyle definitely watched."),
		TEXT("Claudia - from the dark angel at that party to every picnic since: you're Kyle's favorite adventure, his best teammate, and the only escape room he never wants out of."),
		TEXT("For Rumor. The goodest boy. Buddy's keeping your spot warm. <3"),
		TEXT("P.S. Escape room record - unfinished: 1 (mimosas). Finished: 1 (today, together). We don't talk about the first one."),
		TEXT("Press E to keep exploring your little world.")
	};
	return Lines;
}
