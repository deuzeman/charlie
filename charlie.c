#include "charlie.h"

int parse_ildgformat_xml(char *message, paramsIldgFormat *ildgformat)
{
  int read_prec = 0, read_lx = 0, read_ly = 0, read_lz = 0, read_lt = 0;
  char *pos = strtok(message, "<> \n\t");

  if (ildgformat == (paramsIldgFormat*)NULL) {
    return 0;
  }

  while (pos)
  {
    if (!strncmp(pos, "precision", 9)) {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%d", &ildgformat->prec);
      read_prec = 1;
    }
    if (!strncmp(pos, "lx", 2)) {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%d", &ildgformat->lx);
      read_lx = 1;
    }
    if (!strncmp(pos, "ly", 2)) {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%d", &ildgformat->ly);
      read_ly = 1;
    }
    if (!strncmp(pos, "lz", 2)) {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%d", &ildgformat->lz);
      read_lz = 1;
    }
    if (!strncmp(pos, "lt", 2)) {
      pos = strtok(0, "<> \n\t");
      sscanf(pos, "%d", &ildgformat->lt);
      read_lt = 1;
    }
    pos = strtok(0, "<> \n\t");
  }
  return (read_prec && read_lx && read_ly && read_lz && read_lt);
}

int main(int argc, char **argv)
{
  FILE *fp;
  LimeReader *reader;
  
  for (int file_ctr = 1; file_ctr < argc; ++file_ctr)
  {
    fp = fopen(argv[file_ctr], "r");
    reader = limeCreateReader(fp);

    DML_Checksum calculated;
    DML_Checksum read;
    
    n_uint64_t siteSize = 0;
    n_uint64_t volume = 0;

    printf("Checking file %s.\n", argv[file_ctr]);
    
    while (!limeReaderNextRecord(reader))
    {
      if (!strcmp(limeReaderType(reader), "ildg-format"))
      {
        n_uint64_t bytes = limeReaderBytes(reader);
        char *message = (char*)malloc(bytes + 1);
        limeReaderReadData(message, &bytes, reader);
        message[bytes] = '\0';

        paramsIldgFormat ildgFormat;
        parse_ildgformat_xml(message, &ildgFormat);
        siteSize = (ildgFormat.prec == 64) ? (72 * sizeof(double)) : (72 * sizeof(float));
        volume = ildgFormat.lx * ildgFormat.ly * ildgFormat.lz * ildgFormat.lt;

        free(message);
        continue;
      }
      
      if (!strcmp(limeReaderType(reader), "ildg-binary-data"))
      {
        n_uint64_t bytes = limeReaderBytes(reader);
        char *message = (char*)malloc(siteSize);
        
        DML_checksum_init(&calculated);
        
        for (unsigned int rank = 0; rank < volume; ++rank)
        {
          limeReaderReadData(message, &siteSize, reader);
          DML_checksum_accum(&calculated, rank, message, siteSize);
        }

        free(message);
        continue;
      }

      if (!strcmp(limeReaderType(reader), "scidac-checksum"))
      {
        n_uint64_t bytes = limeReaderBytes(reader);
        char *message = (char*)malloc(bytes + 1);
        limeReaderReadData(message, &bytes, reader);
        message[bytes] = '\0';
        
        parse_checksum_xml(message, &read);
        
        free(message);
        continue;
      }
    }

    limeDestroyReader(reader);
    fclose(fp);
    
    printf("  Checksum read:       %08x %08x\n", read.suma, read.sumb);
    printf("  Checksum calculated: %08x %08x\n", calculated.suma, calculated.sumb);
    if ((read.suma == calculated.suma) && (read.sumb == calculated.sumb))
      printf("  File integrity confirmed.\n");
    else
      printf("  WARNING! File corruption detected!\n");
  }
}
