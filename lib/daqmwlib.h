#ifdef __cplusplus
extern "C" {
#endif

  unsigned int *Create_Header(unsigned int *, // header pointer
		      unsigned int *, // footer pointer
		      unsigned int, // event num
		      unsigned int, // event tag
		      unsigned int, // Component Type
		      unsigned int, // Component ID
		      unsigned int, // Size
		      unsigned int, // Sequential Number
		      unsigned int, // User defined data stored in 10th reserved space
		      unsigned int); // User defined data stored in 11th reserved space

#ifdef __cplusplus
};
#endif
