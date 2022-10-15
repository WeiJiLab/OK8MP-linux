echo " "
echo " "
echo "Refreshing Matrix"
cd /www/pages/matrix-gui-2.0/
rm cache/* > /dev/null 2>&1
php-cgi generate.php
cd -
echo "Refresh Complete"
