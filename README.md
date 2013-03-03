Arduino
=======

Just the various Arduino sketches I've been working with. Right now these are your basic "Hello World" kind of sketches.

Setting up Sublime Text 2 on Mac OSX
---

The default Arduino IDE spartan at best. I looked at setting up Eclipse as my default IDE, but ran into some issues with that. Nothing to major, but as I don't like Eclipse in the first place I wasn't to motivated to sort things out, so I abandoned Eclipse as an IDE choice.

The next thing I tried was [Sublime Text](http://www.sublimetext.com/). There is an Ardunio plugin called [Stino](https://github.com/Robot-Will/Stino) that turns Sublime into a not bad IDE. In terms of writing your programs, Stino can pretty much do everything the Arduino IDE can do: compile programs, upload them to your Arduino board, import libraries, etc.

![image](http://raw.github.com/topgenorth/arduino/master/screenshots/sublime_menu_for_arduino.png)

Installing Stino isn't that bad:

1. First make sure that you have downloaded [Arduino 1.0.3](http://arduino.cc/en/Main/Software). I'm not to sure how well Stino will work with the current beta.
2. In Sublime Text, go to Package Control, and type `Install Package`.
3. Search using the keyword `Arduino`, you should see a couple of packages - one contains Stino and the other are a couple of snippets.
4. Next open a `.ino` file in Sublime Text. When you do this the `Arduino` menu should appear in the Sublime toolbard.
5. From the Arduino menu, specify the location of the Ardunio directory, i.e. `/Applications/Arduino.app`.

Done!

To monitor the serial port, I'm using [CoolTerm](http://freeware.the-meiers.org/).  Seems to be "good enough" for what I want to do, and it's free. On Windows, I'd probably use something like puTTY.

[1] Blog post for Stino: [http://kaixin.netii.net/stino-a-sublime-text-2-plugin-for-arduino.html](http://kaixin.netii.net/stino-a-sublime-text-2-plugin-for-arduino.html)