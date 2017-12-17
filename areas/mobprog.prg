#MOBPROGS
#1
if ispc $n
mob oload 3051
give canoe $n
smile $n
endif
~
#3
if ispc $n
emote хихикает.
if isgood $n
emote произносит "Да Вы само совершенство!"
emote вскрикивает "Да Вы защитник всего живого!"
emote спрашивает  "Может, по одной?"
smile $n
else
emote произносит "Ой, А Вы злые!"
giggle $n
emote произносит "Да, Вас добрым не назовеш!"
emote спрашивает "Дать Вам золотой на прЯник? Может подобреете!"
giggle $n
emote лукаво смотрит на $n
endif
endif
~
#4
if ispc $n
say Здгравствуйте батенька ...
emote спрашивает 'На что жалуетесь ? Что резать будем'
emote с грозным видом достает большущий тупой скальпель
look $n
endif
~
#5
if ispc $n 
    emote орет '{mРЯдовой $n ! Ноги вместе, руки на ширину плеч, в Яму прыгом {RМАРШ!{x'{/ 
    mob transfer $n 12536
endif
~
#6
if ispc $n 
    emote орет '{mРЯдовой $n ! {RПОЧЕМУ НЕ НА ПОСТУ?!! {WТри нарЯда ВНЕ ОЧЕРЕДИ!{x'{/
    mob transfer $n 12438
    mob force $n toilet
    mob force $n toilet
    mob force $n sigh
    mob force $n rest
endif
~
#7
say 'Совсем эти рЯдовые распустились...СМИИИИРНО!'
~
#8
say {BЧто за пьяные подростки...{x
emote дает пинок под {Rзад{x $n .
if ispc $n
  mob transfer $n 3336
endif
emote пьет самогонку из бутыля.
emote злобно скалится.
say Пьяниц розвелось !
giggle
~
#9
if ispc $n
  say Плохо ты выглядишь
  mob cast 'giant strength' $n
  say О! Так намного лучше!
endif
~
#10
say И куда ты спешишь ? Умереть ?
emote орет марш в кровать !
if ispc $n
 if hpcnt $n <= 30
   mob force $n north
   mob force $n sleep bed
 endif
endif
~
#11
if ispc $n
 giggle $n
 poke $n
 giggle $n
endif
~
#12
if ispc $n
 say Спасибо тебе, отважный $n !
 say Мой вpаг, $n , создает угpозу коpоне.
 say Эту угpозу необходимо уничтожить!
 say Ищи $n где-то в районе ХрамоваЯ Площадь. !
 say Этот pайон находитсЯ в area Мидгард.
 say У тебЯ есть 13 минут на выполнение заданиЯ.
say Да пpибудет с тобой СИЛА!
endif
~
#13
if ispc $n
   say Спасибо тебе, отважный $n !
 if level $n <= 10
   say Ужасный пpеступник, {RГолова Стpажника{x , сбежал из темницы!
   say С тех поp беглый {RГолова Стpажника{x ,  убил 5 человек!
   say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
   say Ищи {RГолова Стражника{x где-то в pайоне {YФабpичный Склад{x!
   say Этот pайон находитсЯ в area {YФабрика Мобов{x.
  else if level  $n <= 20  and $n > 10
   say Ужасный пpеступник, {RA slaver buyer{x , сбежал из темницы!
   say С тех поp беглый {RA slaver buyer{x ,  убил 7 человек!
   say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
   say Ищи the {RA slaver buyer{x где-то в pайоне {YEntrance to the Slaver's Market{x!
   say Этот pайон находитсЯ в area {YUnderDark{x.
  else if level $n <= 30 and $n > 20 
   say Ужасный пpеступник, {RСтрадающаЯ душа{x , сбежал из темницы!
   say С тех поp беглый {RСтрадающаЯ душа{x ,  убил 9 человек!
   say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
   say Ищи the {RСтрадающаЯ душа{x где-то в pайоне {YЗал Замороженных душ{x!
   say Этот pайон находитсЯ в area {YАд{x.
  else  if level $n <= 55 and $n > 30 
   say Ужасный пpеступник, {RПушистый белый кролик{x, сбежал из темницы!
   say С тех поp беглый {RПушистый белый кролик{x ,  убил 15 человек!
   say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
   say Ищи the {RПушистый белый кролик{x где-то в pайоне {YКаменный колодец{x!
   say Этот pайон находитсЯ в area {YЗатерЯнные Души{x.
 else  if level  $n <= 70 ) and $n > 55 
   say Ужасный пpеступник, {RA drakyri healer{x, сбежал из темницы!
   say С тех поp беглый {RA drakyri healer{x ,  убил 25 человек!
   say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
   say Ищи the {RA drakyri healer{x где-то в pайоне {YWinding path{x!
   say Этот pайон находитсЯ в area {YDrakyri Isle{x.
 else if level  $n > 70 and $n <= 90
  say Ужасный пpеступник, {RАнгел{x, сбежал из темницы!
  say С тех поp беглый {RАнгел{x ,  убил 45 человек!
  say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
  say Ищи the {RАнгел{x где-то в pайоне {YКрай неба{x!
  say Этот pайон находитсЯ в area {YАрмагеддон{x.
 else  if level $n > 90
  say Ужасный пpеступник, {RAn evil shadow person{x из темницы!
  say С тех поp беглый {RAn evil shadow person{x ,  убил 65 человек!
  say Hаказание за это пpеступление - смеpть, и ты пpиведешь его в исполнение!
  say Ищи the {RAn evil shadow person{x где-то в pайоне {YДорога через равнины{x!
  say Этот pайон находитсЯ в area {YWestLand{x.
endif
endif
endif
endif
endif
endif
endif
 say У тебЯ есть 13 минут на выполнение заданиЯ.
 say Да пpибудет с тобой СИЛА!
endif
~
#14
if ispc $n
if sex $n < 2
say Здраствуйте мистер $n!
say Куда хочет путешествовать доблесный $n.
else
say Здраствуйте миледи $n!
say Куда хочет путешествовать доблеснаЯ $n.
endif
say ДлЯ более детальной информации наберите travel list.
endif
~
#20
mob transfer $n 30510
~
#21
if ispc $n
mob force $n sleep
mob back $n
endif
~
#22
if ispc $n
sc
say Как жизнь $n ? 
emote призывает {DДемонов {RАДА{x чтобы они охранЯли твой покой.
say {YОтдохни после боЯ{x
comf $n
endif
"
~
#28
~
#29
~
#50
if ispc $n
if level $n <= 17
say Интересные новости услышал Я у вас в таверне.
endif
endif
~
#51
if ispc $n
say Заходи, выпей пивка.
endif
~
#52
if ispc $n
if level $n <= 17
smile
tell $n Слышал Я интересные новости.
scan
tell $n ГоворЯт, что у какого-то султана горе, ему нужна помощь.
nod $n
endif
endif
~
#53
if ispc $n
if level $n <= 17
cry $n
tell $n Подлые воры похитили все мои сокровища, а за одно и ключ от гарема.
tell $n Ты не мог бы помочь мне разыскать ключ?
tell $n Прошу тебЯ, найди того вора.
shake $n
endif
endif
~
#54
if ispc $n
if level $n <= 17
smile $n
tell $n Ну Я не прочь услужить ... за небольшое вознаграждение.
endif
endif
~
#55
if ispc $n
if level $n <= 17
stare
endif
endif
~
#56
if ispc $n
if level $n <= 17
sigh
tell $n Ты опоздал. Я выменЯл это барахло у менЯлы на ... не скажу что.
wave
endif
endif
~
#57
if ispc $n
if level $n <= 17
tell $n Да, был тут недавно один тип.
tell $n Принес мне какой-то ключ. Я подумал, что этот ключ подойдет к одной шкатулочке, но Я ошибсЯ.
blush
tell $n Бери, он мне не нужен.
give key $n
wave $n
endif
endif
~
#58
if ispc $n
if level $n <= 17
smile
say Спасибо тебе, чужестранец.
say Я уже потерЯл надежду на то, что увижу свою любимую Наташку.
say Прими от менЯ этот скромный подарок.
mob oload 30163
give blue $n
shake $n
endif
endif
~
#66
if ispc $n
if level $n <= 17
say {cКоррешш, помогги.. ломит, ужжассно.. мне б ша глюкнуть че-ттоо..
say х-хемлока, например, бахнуть.. 
say {cСлушай, если достанешь мне хоть че-нить - ЯЯ в долгу не останусь!..{x
endif
endif
~
#67
say {GО! {WН{cи{Wш{cт{WЯЯ{cк{Y!{x
say {YРодной {cхемлок{Y!{x
say {GСпасибо, чувак.. $n, ты мне просто спасаешь жизнь!{x
emote глюкаетсЯ
dri hemlock
mob junk hemlock
echo {yПапу Смурфа{G плющщит{x
say {CУЛЕТ!{W ЛЕТИМ СО МНОЙ!{x
mob cast fly
mob cast fly $n
~
#1001
if ispc $n
if sex $n > 1
say {CБрильЯнты {Yлутьшие друзьЯ девушки.{x
endif
if carries $n 2423
mob force $n drop diamond
mob force $n sac diamond
mob oload 12453 15 'R'
say {YДержи взамен.{x
mob force $n take ring
endif
~
#1002
if ispc $n
if carries $n 6519
mob force $n give pills nichole
mob junk pills
mob oload 3377 15 'R'
emote ронЯет {Cдрагоценный {Wкамешек.{x
endif
endif
~
#1003
if ispc $n
if carries $n 29010
else
if rand 30
say Давно не виделись $n.
say Ты наверно пришел уладить то громкое дело с пропажей?
say Наслышан наслышан.
say Чую что здесь замешан один из мидгарской банды воров.
say Я помогу тебе втеретьсЯ к ним в доверие, но и ты помоги мне.
say Я давно мечтаю о топоре достойном королЯ.
endif
endif
endif
~
#1004
if ispc $n
mob junk axe
say Эта метка означает, что ты член гильдии воров. А теперь прощай.
mob oload 77 1
give metka $n
endif
~
#1006
if ispc $n
if carries $n 77
mob force $n give metka thief
say О да Я вижу ты один из наших.
say Говориш товар не дошел до получателЯ вот беда, вот беда.
say Я сейчасже высылаю своих людей на восточную дорогу разыскивать {Gс{gа{Gп{gо{Gж{gк{Gи.{x
say Султан же гневатьсЯ будет.
mob junk metka
say Думаю чтобы убить того кто перехватил караван потребуетсЯ хорошее оружие.
say Такое как у моего брата из подземного царства.
endif
endif
~
#1007
if ispc $n
if carries $n 16071
mob oload 5099 60 'R'
mob zecho "{YИз распоротого живота червЯ вываливаютсЯ остатки верблюда и {Gс{gа{Gп{gо{Gг{gи{x"
endif
endif
~
#1008
if ispc $n
if carries $n 30386
mob force $n give eye mole
mob junk eye
mob zecho Ты передаеш капли кроту.
say Спасибо тебе отважный герой.
mob oload 11709 1 'R'
say Вот Я тут нашел одну штучку, может тебе пригодитсЯ.
else
say Отважный рыцарь помоги мне вновь обрести зрение, а то совсем ничего не вижу.
say Я слышал в далекой стране есть настой, который очень помогает.
say Уж Я в долгу не останусь
endif
endif
~
#1009
if ispc $n
if mobhere 3090
say О ты привел мне сородича.
say Теперь мне будет с кем играть!
emote {Bм{Wу{Bр{Wр{Bч{Wи{Bт.{x
mob force $n stopfol
mob purge kitten
mob oload 12012 54 'R'
mob zecho "{WКот бросает книгу {Yсказок{x"
else
say Как же мне надоели эти {Yсказки{x.
say Так хочетсЯ с кемнибудь поиграть.
say Укусить за хвост, цапнуть за ухо, потЯнуть за ус.
endif
endif
~
#1010
if ispc $n
if carries $n 12012
say О ты принес ее!
mob force $n drop book
emote углублЯетсЯ в чтение.
mob oload 2235 1 'R'
mob purge book
else
say Доброго днЯ странник.
say Не встречал ли ты на своем пути редких и ценных книг?
say А может ты бывал в месте которое Я стал считать своим домом?
say Там где Я научилсЯ всему.
say Ты себе даже не представлЯеш сколько там книг!
say Может принесеш мне парочку? А Я тебе взамен дам ключик от {Yс{Wок{Yр{Wо{Yв{Wи{Yщ{Wн{Yи{Wц{Yы.{x
endif
endif
~
#1011
if ispc $n
if carries $n 2235
say Ты нашел их! Мои ключики!
say Давай их скорее сюда.
mob remove $n 2235
mob oload 23056 1 'R'
say Вот держи взамен.
else
say Боже мой, боже мой!
emote рвет на себе волосы.
say ПотерЯл! Ключи потерЯл!
say Чтобы Я хоть раз еще дал в долг этим титановым парнЯм?
say Никогда!
endif
endif
~
#1012
if ispc $n
if isevil $n
if carries $n 14019
mob remove $n 14019
say О ты убил его! Наконец!
say Теперь ничто не может помешать {DТ{xЬ{DМ{xЕ!{x
mob oload 20031 1 'R'
else
say Я вижу твое серце принадлежит {DТЬМЕ.{x
say Думаю ты не откажешсЯ покончить со средоточением света - {WМихаилом?{x
endif
endif
endif
~
#1013
if ispc $n
if isgood $n
if carries $n 20024
mob remove $n 20024
mob cast 'shield' $n
mob cast 'armor' $n
say Пусть будет {WСВЕТ!{x
else
say Я вижу твое серце принадлежит СВЕТУ.
say Помоги мне избавитьсЯ от исчадиЯ ада - {DДаркена Рахла.{x
endif
endif
endif
~
#1014
if ispc $n
if carries $n 12452
if carries $n 12453
mob remove $n 12453
mob remove $n 12452
say Молодец солдат!
say Повышаю тебЯ в звании!
if wears $n 12429
mob oload 12430
else
mob oload 12429
endif
give straps $n
endif
else
say Вольно!
say Что потренироватьсЯ пришел?
say Похвально, похвально.
if rand 50
say А у тебЯ нет случайно {Yобручального колечка{x?
say Кстати сбегай за минералкой!
endif
endif
endif
~
#1015
if level $n > 90
mob junk string
endif
~
#1016
if ispc $n
if carries $n 3001
else
say Ох одна работа... ничего кроме нее не вижу.
say Даже за "Мидгарским" пивом сходить некогда.
say Может ты принесеш бутылочку?
endif
endif
~
#1017
if ispc $n
mob junk bottle
mob gecho Флойд заплом выпивает пиво.
say Хорошо пошло!
mob cast haste $n
endif
~
#1018
if ispc $n
if carries $n 3012
else
say О путник ты не из Мидгара случайно?
say Да?! Какое счастье!
say А ты не захватил с собой славный мидгарский рулет?
say КакаЯ жалость, что нет.
endif
endif
~
#1019
if ispc $n
mob junk cinnamon
mob zecho "Хиерофант смачно чавкает"
say ВкуснЯтина!
mob cast sanctuary $n
endif
~
#1020
if ispc $n
if carries $n 9520
else
say Ох и много же всЯкого сброду развелось в нашем городе.
say Проходу честным горожанам не дают.
say Без щита уже и ходить нельзЯ.
say Я слышал в {CНовом Талосе{G хорошие щиты делают.{x
endif
endif
~
#1021
if ispc $n
mob junk shield
mob gecho Мэр пытаетсЯ поднЯть щит, но понимает что слишком слаб длЯ этого.
say Эх.... не так он хорош этот щит, как расказывали.
say Но вот тебе награда.
mob cast shield $n
endif
~
#6700
if ispc $n
if isgood $n
say {WЗдорово, {Y$n{R!!!{x
say {MСадись, будешь играть {Wбелыми{x
if sex $n < 2
sc
say {WКстати, ты пиво "{D22{x{W" принёс?{x"
mob force $n say {WДа, есть же!{x
shake $n 
else
sc 
say {WКстати, ты пиво "{D22{x{W" принесла?{x"
mob force $n say {WНу ты жлоб! Поцелуй-ка меня в ...{x
kiss $n
endif
else
say {DЗаходи {Y$n{D, я тебя давно жду.{x
giggle $n
say {MСадись, будешь играть {Dчёрными{x
if sex $n < 2
sc
say {WКстати, ты пиво "{D22{x{W" принёс?{x"
mob force $n say {WСегодня только белая!{x
bow $n 
else
sc 
say {WКстати, ты пиво "{D22{x{W" принесла?{x"
mob force $n say {WКонечно, дорогой!{x
smile $n
endif
endif
endif
"
~
#10131
if clan $n 'chaos'
    mob transfer $n 26301
endif
~
#26310
mob echo {x
if clan $n 'chaos'
say Привет, привет... гулЯешь? Ты тут смотри - ничего зрЯ не трогай...
mob echo {DДух огромной серой Крысы{x внимательно следит за тобой...
else
say Это что еще такое?
say А ну бегом отсюда...
say Чтобы через секунду Я тут тебЯ не видел!
mob delay 2
endif
~
#26311
if hastarget $i
growl
say Ты еще ЗДЕСЬ?!?!
say Ну, Я тебЯ предупреждал...
backstab $q
else
say Гхм...
mob forget
endif
~
#26320
say Хм...
if clan $n 'chaos'
tell $n Смотри только, веди себЯ прилично...
unlock down
open down
mob force $n down
mob at $n emote запирает дверь за тобой.
close down
lock down
end
endif
tell $n Ты недостоин...
~
#26400
say Hi!
~
#27000
if clan $n 'jumandji'
say Здравствуй, родной! :){x
kiss $n
else       
say {GИ чего мы тут делаем? {RАА{G? А ну бегом.. отсюда..{x
murder $n        
say {GЧто не бежитсЯ? :E{x
endif
~
#27001
if clan $n jumandji
smile $n
nod $n
else
emote хмурит брови...
murder $n
grin $n
endif
if name $n 'Dragon'
say '{GПриветик... как дела? кого-то ишещь? :){x'
wave $n
mob transfer $n 3054
endif
if name $n 'Astellar'
say 'Мое почтение, мастер...'
emote преклонЯет колено.
endif
~
#0

#$
