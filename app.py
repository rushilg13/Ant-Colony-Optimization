from flask import Flask, render_template, request
import requests

app = Flask(__name__)

APIKEY = "0oiqumUfEAMI7EAdjABZ3SPO0QPmrY7uY4PNYHUl1NazJoGEXomMh8fYUlFIwS8V"

@app.route("/")
def home():
    return render_template("home.html")


@app.route("/submit", methods=['POST'])
def sent():
    num = 3
    dist_matrix = [[-1, -1, -1], [-1, -1, -1], [-1, -1, -1]]
    loc_arr = []
    loc1 = request.form['loc1']
    loc2 = request.form['loc2']
    loc3 = request.form['loc3']
    # loc4 = request.form['loc4']
    # loc5 = request.form['loc5']
    # loc6 = request.form['loc6']
    # loc7 = request.form['loc7']
    # loc8 = request.form['loc8']
    loc_arr.append(loc1)
    loc_arr.append(loc2)
    loc_arr.append(loc3)
    # loc_arr.append(loc4)
    # loc_arr.append(loc5)
    # loc_arr.append(loc6)
    # loc_arr.append(loc7)
    # loc_arr.append(loc8)
    
    for i in range(num):
        for j in range(num):
            URL = "https://www.zipcodeapi.com/rest/0oiqumUfEAMI7EAdjABZ3SPO0QPmrY7uY4PNYHUl1NazJoGEXomMh8fYUlFIwS8V/distance.json"
            if i == j:
                dist_matrix[i][j] = 0
            else:
                URL = URL + "/" + loc_arr[i] + "/" + loc_arr[j] + "/km"
                r = requests.get(url = URL)
                data = r.json()
                dist_matrix[i][j] = data['distance']
    print(dist_matrix)
    
    f = open("data.txt", "a")
    f.write(str(num))
    f.write("\n")
    for i in range(num):
        for j in range(num):
           f.write(str(int(dist_matrix[i][j])))
           f.write(" ")
        f.write("\n")
    f.close()
    
    return render_template("home.html")

if __name__ == "__main__":
    app.run(debug=True)