sed "/:\//{
    s/^ */\&nbsp;\&nbsp;\&nbsp;\&nbsp;<a href=\"/
    s/ *$/\">/
    p
    s/.*<a href=\"//
    s/\">/<\/a><br>/
    }" $1 > $2
