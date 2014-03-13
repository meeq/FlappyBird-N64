typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;
typedef volatile uint64_t vu64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef uint64_t u64;
typedef signed short s16;

/* Video definitions */

#define BUFFERING_DOUBLE 2
#define BUFFERING_TRIPLE 3

/* Audio definitions */

#define FREQUENCY_44KHZ 44100
#define STEREO_PCM_SAMPLE_SIZE (2 * sizeof(s16))

struct raw_pcm {
  int numSamples;
  s16 *sampleData;
};
typedef struct raw_pcm raw_pcm_t;
