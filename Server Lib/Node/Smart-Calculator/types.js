const ePACKET = {
	LOGIN : 0,							// Login com server Nodejs
	HAND_SHAKE : 1,						// Hand Shake - Login com server Nodejs
	COMMAND_SERVER : 2,					// Server manda comando para o Smart Server do Nodejs
	REPLY_SERVER : 3,					// Resposta para o Server
	REPLY_SMART : 4,					// Resposta para o Smart Calculator
	REPLY_STADIUM : 5,					// Resposta para o Stadium Calculator
	COMMAND_SMART_PING : 6,				// Ping
	COMMAND_SMART_LIST_MACRO : 7,		// List Macro
	COMMAND_SMART_CALC : 8,				// Calcula
	COMMAND_SMART_MACRO : 9,			// Exec Macro
	COMMAND_SMART_EXPRESSION : 10,		// Exec Expression
	COMMAND_STADIUM_PING : 11,			// Ping
	COMMAND_STADIUM_LIST_MACRO : 12,	// List Macro
	COMMAND_STADIUM_LIST_CALC : 13,		// List Calculator
	COMMAND_STADIUM_OPEN_CALC : 14,		// Abre Calculator
	COMMAND_STADIUM_CALC : 15,			// Calcule
	COMMAND_STADIUM_MACRO : 16,			// Exec Macro
	COMMAND_STADIUM_EXPRESSION : 17,	// Exec Expression
};

module.exports = {
    ePACKET: ePACKET
}