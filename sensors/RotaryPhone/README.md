# Rotary Phone

I have an old (but inauthentic) rotary phone which has a keypad interface. I'm going to use this to send commands to homeassistant!

## Notes

Uses Keypad library to setup 4x4 row/col matrix, where one of the rows is entirely dedicated to the middle 'redial' button. For now, homeassistant seems to interpret the received keys as ascii codes, but it's simple to translate these back.

## To Do

At present the arduino never sleeps, because it's slightly complicated to combine sleep/interrupt with the keypad library (the keypad library operates by repeatedly cycling through rows and looking for columns which switch). There is a recipe for this provided by [Nick Gammon](http://www.gammon.com.au/forum/?id=11497&reply=4#reply4), so I will try to implement something similar.


