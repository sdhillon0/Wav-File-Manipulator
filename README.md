# Wav-File-Manipulator

# Description
- remvocals.c is a command-line program desigend to remove vocals from a audio file in WAV format. To use it simply pass in the path/name of the WAV file.
- addecho.c is a command-line program designed to add echo effects to a specified audio file in WAV format and will be the main focus of this project. It allows users to manipulate the delay and intensity of the echo effect. The program takes an input WAV file and produces an output WAV file with echo effects applied.

# Usage
- addecho [OPTION]... FILE1 FILE2

# Options
- -d=DELAY: Specifies the delay between each echo effect in samples. Echos occur after DELAY samples of the original sound. The delay value must start with an integer strictly between 0 and 2147483547.
- v=VOLUME_MULTIPLIER: Governs the intensity of the echo relative to the original sound's volume. Echos occur at the strength of 1/VOLUME_MULTIPLIER of the original sample (rounded down). The volume multiplier value must start with an integer strictly between 0 and 2147483547.

# Defaults
- If options are not specified, the following defaults are applied:
    - Delay (-d): 8000 samples
    - Volume Multiplier (-v): 4

# Return Values
- 0: The program executed successfully without any errors.
- 1: An error occurred during execution due to improper usage of command-line arguments, file-related issues, or errors during the audio data processing.

# Examples
- addecho input.wav output.wav: Create an audio file named output.wav with default delay and volume multiplier.
- addecho -d 10000 -v 2 input.wav output.wav: Create an audio file named output.wav with custom delay and volume multiplier.
- addecho -d 10000 -v 2 -v 3 -d 9000 input.wav output.wav: Create an audio file named output.wav with multiple custom delay and volume multiplier settings.

# Warnings
- Invalid Option: An unidentifiable option is given.
- Invalid Value for Option: The provided value for an option is forbidden.
- File Overwriting: The output file will overwrite any existing content with the same name.
- File Name Conflict: The input and output file names are the same.
- File Access Issues: Unable to open input, output, or both files.
- Header Reading Error: The input file does not include a legal header for WAV files.
- Output Writing Failure: Errors occurred while writing the output file.
- File Closure Failure: Unable to close input or output file, resulting in undefined output contents.
