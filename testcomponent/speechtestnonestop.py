import pyttsx3

engine = pyttsx3.init()
voices = engine.getProperty('voices')
engine.setProperty('voice', voices[len(voices) - 2].id)
        
def speak(audio):
    print('Computer: ' + audio)
    engine.say(audio)
    engine.runAndWait()

if __name__ == '__main__':
    speak('Hello Sir, I am your digital assistant LARVIS the Lady Jarvis!')
    speak('How may I help you?')