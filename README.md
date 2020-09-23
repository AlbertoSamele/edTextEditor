# edTextEditor

The project consists in the implementation of a simple text editor. The editor considers a document as a sequence of lines, of arbitrary dimension, numbered starting from one.
The text editor interface is made out of new-line-terminated textual instructions. The instructions can be followed by a portion of text, made out of one or more lines, followed by a . (full stop character), which appears as the only character on the following line. The instructions are made out of a single letter, eventually preceded by one or two ingeres.
The text editor interface is freely inspired from the tradional ED editor.
In some instructions, the integers appear as address specifiers. More precisely, an address specifier is a number n, in decimal notation, that signifies the start of the n-th line; the first line of text has address 1.
The supported instructions are the following, assuming that ind1,ind2 represent two address specifiers so that ind1 ≤ ind2 and the brackets are simply introduced to simplify the reading and comprehension of this text, but won't be included in the actual instruction:


• (ind1,ind2)c
Changes the text in between ind1 and ind2. The text that follows the command must be made out of a number of lines n ? ind2-ind1+1. ind1 must be either an address currently present in the text, or the first adress immediately after the last line in the text (or 1 if the text is still empty).

• (ind1,ind2)d
Deletes the lines in between ind1 and ind2, shifting upwards the lines after ind2 (if any). The deletion of a non-existing line has no effect on the text.

• (ind1,ind2)p
Prints out the lines in between ind1 and ind2. Wherever no lines is present in the text at the target line position, a single line containg the character '.' is printed instead, followed by a new-line character.

• (number)u
Undos a number of instructions (c or d) equal to the one specified between the brackets, assuming that 'number' is an integer strictly greater than zero). A series of consecutive undo instructions undos a number of instructions equal to the sum specified by each one of them. If the number of instructions that need to be undone is greater than the number of instructions actually executed, all instructions are undone. The execution of an instruction that modifies the text (c or d) after an undo instruction cancels the effects of the undone commands irremediably. Note that even instructions that have no effect (like the deletion of an empty text block) are counted when doing n undos

• (number)r
Reverts the effect of an undo for a number of instructions equal to 'number', starting from the current text version (redo function). Note that 'number' must be an integer strictly greater than zero.

• q
Quits the editor execution
