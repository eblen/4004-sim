enum PARSE_ERROR{NONE, BAD_MNEMONIC, TOO_FEW_ARGS, TOO_MANY_ARGS, ARG1_OUT_OF_RANGE, ARG2_OUT_OF_RANGE};

// Return the instruction size (either 1 or 2 bytes) for the given mnemonic
// If not found, return 0
unsigned int instr_size(const std::string &op);

// Convert a single assembly instruction to ebin
std::pair<std::string, PARSE_ERROR> assembly_to_ebin(const std::string &op, const std::vector<unsigned int> &args);
