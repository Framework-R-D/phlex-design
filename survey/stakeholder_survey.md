# Phlex Framework - Stakeholder Feedback Survey (Post v0.1.0)

## Purpose
This survey helps the Phlex development team understand user experiences, prioritize features for upcoming releases, and improve the overall framework design.

## Survey Structure
- **Rating Scale**: 1 (Poor/Strongly Disagree) to 5 (Excellent/Strongly Agree)
- **Multiple Choice**: Select one or more options as indicated
- **Yes/No/Not Applicable**: Binary choices with N/A option
- **Open Text**: For detailed feedback (optional but encouraged)

---

## Part 1: Respondent Profile

### Q1. What is your primary role? (Select one)
- [ ] Staff Scientist
- [ ] University Professor
- [ ] Software Developer
- [ ] Graduate Student
- [ ] Postdoc
- [ ] Other (please specify): ___________

### Q2. How would you describe your experience level with C++? (Select one)
- [ ] Expert (10+ years, deep language knowledge)
- [ ] Advanced (5-10 years, comfortable with modern C++)
- [ ] Intermediate (2-5 years, working knowledge)
- [ ] Beginner (< 2 years)
- [ ] No C++ experience

### Q3. How would you describe your experience level with Python? (Select one)
- [ ] Expert (10+ years, deep ecosystem knowledge)
- [ ] Advanced (5-10 years, proficient)
- [ ] Intermediate (2-5 years, working knowledge)
- [ ] Beginner (< 2 years)
- [ ] No Python experience

### Q4. Which best describes your use case? (Select all that apply)
- [ ] Near detector simulation and/or reconstruction
- [ ] Far detector simulation and/or reconstruction
- [ ] Physics algorithm development and testing
- [ ] Other (please specify): ___________

### Q5. How familiar are you with the higher-order function paradigm (transform, fold, unfold, observe)? (1-5 scale)
1 - Not at all familiar | 2 - Slightly familiar | 3 - Moderately familiar | 4 - Very familiar | 5 - Extremely familiar | N/A

---

## Part 2: Installation & Setup Experience

### Q6. How easy was it to install and set up Phlex? (1-5 scale)
1 - Very Difficult | 2 - Difficult | 3 - Moderate | 4 - Easy | 5 - Very Easy | N/A - Haven't installed

### Q7. Which installation method did you use? (Select one)
- [ ] Spack package manager
- [ ] Manual build from source
- [ ] Container (Docker, Podman, etc.)
- [ ] Pre-built binaries
- [ ] Other (please specify): ___________
- [ ] Haven't installed yet

### Q8. How clear and helpful was the installation documentation? (1-5 scale)
1 - Very Unclear | 2 - Unclear | 3 - Adequate | 4 - Clear | 5 - Very Clear | N/A

**Comments on installation experience**:
_________________________________________________

---

## Part 3: Core Functionality Assessment

### Q9. Which Phlex features have you used? (Select all that apply)
- [ ] C++ transform algorithms
- [ ] C++ observe algorithms
- [ ] C++ fold algorithms
- [ ] C++ unfold algorithms
- [ ] Python transform algorithms
- [ ] Python observe algorithms
- [ ] Data-product providers (C++)
- [ ] Jsonnet configuration
- [ ] Data-product handles
- [ ] Data-layer hierarchy
- [ ] None yet (planning to)

### Q10. Rate the ease of registering algorithms (1-5 scale)

**C++ algorithms**: 1 - Very Difficult | 2 - Difficult | 3 - Moderate | 4 - Easy | 5 - Very Easy | N/A

**Python algorithms**: 1 - Very Difficult | 2 - Difficult | 3 - Moderate | 4 - Easy | 5 - Very Easy | N/A


**Comments on core functionality**:
_________________________________________________

---

## Part 4: Documentation & Learning Resources

### Q11. Rate the quality of the following documentation (1-5 scale each):

**README and getting started guide**: 1 - Poor | 2 - Fair | 3 - Good | 4 - Very Good | 5 - Excellent | N/A

**phlex-examples repository**: 1 - Poor | 2 - Fair | 3 - Good | 4 - Very Good | 5 - Excellent | N/A

### Q12. What documentation improvements would be most valuable? (Select top 3)
- [ ] More code examples
- [ ] Tutorial series (beginner to advanced)
- [ ] API reference completeness
- [ ] Migration guides (from other frameworks)
- [ ] Best practices guide
- [ ] Other (please specify): ___________

### Q13. How easy was it to get started with the phlex-examples repository? (1-5 scale)
1 - Very Difficult | 2 - Difficult | 3 - Moderate | 4 - Easy | 5 - Very Easy | N/A

**Comments on documentation**:
_________________________________________________

---

## Part 5: C++/Python Interoperability

### Q14. Have you used C++/Python interoperability features? (Select one)
- [ ] Yes, Python consuming C++ products
- [ ] Yes, C++ consuming Python products
- [ ] Yes, both directions
- [ ] No, but plan to
- [ ] No, not needed for my use case

### Q15. Are there type-conversion or data-marshaling issues between C++ and Python? (Select one)
- [ ] No issues
- [ ] Minor issues (workarounds available)
- [ ] Significant issues (limiting functionality)
- [ ] Major blockers
- [ ] Haven't tested this feature

**Comments on interoperability**:
_________________________________________________

---

## Part 6: Missing Features & Pain Points

### Q16. What critical features should be prioritized for future releases? (Select all that apply)
- [ ] More Python data types supported
- [ ] Processing non-trivial data layer hierarchies
- [ ] Additional configuration languages
- [ ] Support for persistent references and associations
- [ ] Ability to create user-defined framework drivers
- [ ] Better debugging/profiling tools
- [ ] GPU integration with the framework
- [ ] Safe access to thread-unsafe resources
- [ ] Other (please specify): ___________

### Q17. What is your biggest pain point with Phlex v0.1.0? (Open text)
_________________________________________________


### Q18. What ONE feature would provide the most value in the next release? (Open text)
_________________________________________________

**Additional comments on missing features**:
_________________________________________________


### Q19. Any other comments, suggestions, or feedback?
_________________________________________________

---

## Thank You!

Your feedback is crucial for improving Phlex. The development team will analyze all responses to prioritize work for upcoming releases.

**Survey completion time**: Approximately 10-15 minutes

**Contact for follow-up** (optional): ___________

---

## For Survey Administrators: Analysis Guide

### Quantitative Metrics for Visualization

The survey is designed to produce the following plot types:

1. **Stacked Bar Charts**:
   - Q1: Respondent role distribution
   - Q2-Q3: Experience level distribution
   - Q9: Feature usage patterns
   - Q16: Missing features by priority

2. **Rating Distribution Plots** (1-5 scale questions):
   - Q6, Q8, Q10, Q11, Q13, Q15, Q22, Q24, Q26, Q30, Q35-Q36, Q39, Q41-Q42
   - Can show as histograms, box plots, or violin plots

3. **Heatmaps**:
   - Q36: Priority matrix across development areas
   - Q35: Integration importance matrix
   - Cross-tabulation: Experience level vs. satisfaction ratings

4. **Percentage/Pie Charts**:
   - Q7: Installation method distribution
   - Q14: Binary/categorical distributions

5. **Multi-select Analysis** (bar charts showing frequency):
   - Q4: Use case distribution
   - Q9: Feature usage
   - Q12: Documentation needs
   - Q16: Missing features
   - Q33: Production readiness factors
   - Q34: Integration requirements
   - Q38: Support preferences
   - Q40: Participation interest

6. **Correlation Analysis**:
   - Experience level (Q2-Q3) vs. ease of use ratings (Q10)
   - Role (Q1) vs. feature priorities (Q16)
   - Use case (Q4) vs. performance ratings (Q15)
   - Documentation ratings (Q11) vs. overall satisfaction (Q41)

7. **Time Series** (if survey is repeated):
   - Track improvements in satisfaction scores
   - Feature adoption over time
   - Issue resolution trends

8. **Word Clouds/Theme Analysis** (open-ended responses):
   - Q17: Pain points
   - Q18: Most valuable features
   - Q43: Top improvement suggestions
   - Q19: General feedback

### Key Performance Indicators (KPIs)

- **Net Promoter Score**: Based on Q31 (recommend to colleagues)
- **Adoption Intent**: Q32 (production usage plans)
- **Feature Completeness**: % of users satisfied with feature set
- **Documentation Quality**: Average across Q11 ratings
- **Overall Satisfaction**: Q41 mean score
- **Developer Experience**: Average of Q24 ratings
- **Ease of Use**: Average of installation and setup questions

### Segmentation for Analysis

- By role (Q1)
- By experience level (Q2-Q3)
- By use case (Q4)
- By installation method (Q7)
- By feature usage (Q9)
- By production status (Q32)

This segmentation allows for targeted improvements based on user personas.
