- add species differential
-  
struct Agent {
	float2 position;
	float angle;
	int4 speciesMask;
	int speciesIndex;
};

struct SpeciesSettings {
	float moveSpeed;
	float turnSpeed;

	float sensorAngleDegrees;
	float sensorOffsetDst;
	int sensorSize;
	float4 colour;
};
