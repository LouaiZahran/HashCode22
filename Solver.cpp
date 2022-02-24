#include<bits/stdc++.h>
using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> distribute(0, (int)1e5);

//Returns a random number from the mt19937
int getRandom(long long upperBound){
    return distribute(gen)%(upperBound);
}

class Skill{
public:
    string name;
    int level;

    Skill(){}

    Skill(string name, int level){
        this->name = name;
        this->level = level;
    }

    static Skill* readSkill(){
        string name;
        int level;
        cin>>name>>level;
        return new Skill(name, level);
    }

    void print(){
        cout<<name<<": "<<level<<"\n";
    }
};

class Contributor{
public:
    string name;
    vector<Skill> skills;

    Contributor(string name, vector<Skill> skills){
        this->name = name;
        this->skills = skills;
    }

    static Contributor* readContributor(){
        string name;
        int numOfSkills;
        vector<Skill> skills;
        cin>>name>>numOfSkills;
        for(int i=0; i<numOfSkills; i++)
            skills.push_back(*Skill::readSkill());
        return new Contributor(name, skills);
    }

    bool hasSkill(string name){
        for(Skill skill: skills)
            if(name == skill.name)
                return true;
        return false;
    }

    Skill* getSkill(string name){
        for(Skill& skill: skills)
            if(name == skill.name)
                return &skill;
        return new Skill();
    }

    void print(){
        cout<<name<<"\n";
        for(Skill s: skills)
            s.print();
    }
};

class Project{
public:
    string name;
    int d, s, b, r;
    vector<Skill> skills;

    Project(){};

    Project(string name, int d, int s, int b, int r, vector<Skill> skills){
        this->name = name;
        this->d = d;
        this->s = s;
        this->b = b;
        this->r = r;
        this->skills = skills;
    }

    static Project* readProject(){
        string name;
        int d, s, b, r;
        vector<Skill> skills;
        cin>>name>>d>>s>>b>>r;
        for(int i=0; i<r; i++)
            skills.push_back(*Skill::readSkill());
        return new Project(name, d, s, b, r, skills);
    }

    void print(){
        cout<<name<<" "<<d<<" "<<s<<" "<<b<<" "<<r<<"\n";
        for(Skill s: skills)
            s.print();
    }
};

class Assignment{
public:
    Project project;
    vector<Contributor> contributors;

    Assignment(){}

    Assignment(Project project, vector<Contributor> contributors){
        this->project = project;
        this->contributors = contributors;
    }

    ~Assignment(){
        contributors.clear();
    }

    bool verify(){
        for(Skill requiredSkill: project.skills){
            bool skillSatisfied = false;

            for(Contributor contributor: contributors)
                for(Skill availableSkill: contributor.skills)
                    skillSatisfied|= (availableSkill.name == requiredSkill.name && availableSkill.level >= requiredSkill.level);

            if(!skillSatisfied)
                return false;
        }
        return true;
    }

    void print(){
        cout<<project.name<<"\n";
        for(Contributor contributor: contributors)
            cout<<contributor.name<<" ";
        cout<<"\n";
    }
};

class Solution{
public:
    vector<Assignment> assignments;
    long long score;
    int cnt;

    Solution(){
        this->assignments.clear();
        this->score = 0;
    }

    Solution(vector<Assignment> assignments){
        this->assignments = assignments;
        this->score = 0;
    }

    ~Solution(){
        assignments.clear();
    }

    bool verify(){
        for(auto assignment: assignments)
            if(!assignment.verify())
                return false;
        return true;
    }

    void simulate(){
        long long currentTime = 0;
        for(Assignment& assignment: assignments){
            if(!assignment.verify()){
                score = -1;
                break;
            }
            cnt++;
            currentTime += assignment.project.d;
            score += max(0LL, assignment.project.s - max(0LL, currentTime - assignment.project.b + 1));
            update(assignment);
        }
    }

    void update(Assignment& assignment){
        for(Skill skill: assignment.project.skills){
            for(Contributor& contributor: assignment.contributors)
                if(contributor.hasSkill(skill.name) && (contributor.getSkill(skill.name)->level == skill.level
                                                    ||  contributor.getSkill(skill.name)->level == skill.level - 1))
                    contributor.getSkill(skill.name)->level++;

        }
    }

    void print(){
        cout<<cnt<<"\n";
        for(Assignment assignment: assignments)
            assignment.print();
    }
};

int c, p;
Contributor** contributors;
Project** projects;

void readInput(){
    freopen("./Test/c_collaboration.in.txt", "r+", stdin);
    freopen("./Test/c_out.txt", "w+", stdout);

    cin>>c>>p;

    contributors = (Contributor**) malloc(sizeof(Contributor*) * c);
    projects = (Project**) malloc(sizeof(Project*) * p);

    for(int i=0; i<c; i++)
        contributors[i] = Contributor::readContributor();
    for(int i=0; i<p; i++)
        projects[i] = Project::readProject();
}

Solution* generate(){
    Solution *trialSolution = new Solution();
    random_shuffle(projects, projects + p);

    for(int i=0; i<p; i++){
        Project* project = projects[i];
        random_shuffle(contributors, contributors + c);
        vector<Contributor> assignees;
        for(int i=0; i<project->r; i++)
            assignees.push_back(*contributors[i]);
        trialSolution->assignments.push_back(*new Assignment(*project, assignees));
    }

    return trialSolution;
}

Solution* solve(int numOfSeconds){
    time_t start = time(NULL);

    Solution *bestSolution = new Solution();
    while(time(NULL) - start <= numOfSeconds){
        Solution *trialSolution = generate();
        trialSolution->simulate();
        if(trialSolution->score > bestSolution->score)
            delete bestSolution, bestSolution = trialSolution;
        else
            delete trialSolution;
    }

    return bestSolution;
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    readInput();
    Solution *sol = solve(20);  //Try randomizing solutions for 20 seconds and print the best solution
    sol->print();

    return 0;
}
